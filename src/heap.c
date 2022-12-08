#include "heap.h"

#include "debug.h"
#include "mutex.h"
#include "tlsf/tlsf.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>

/**
 * \brief A structure to represent "arenas" within a heap.
 * 
 * The struct arena_t is used to represent specific "arenas", pools of contiguous memory within the heap that can be divided up into blocks, 
 * to then be allocated and destroyed as necessary. Arenas are commonly split up based on tasks within the game, 
 * so that a task's memory can be freed all at once when that task is complete, but in this specific implementation we just assign new heaps 
 * whenever we run out of memory in the current heap.
 */
typedef struct arena_t
{
	pool_t pool; /**< pool is the specific set of memory assigned to this arena. A given arena may only contain one pool, but an instance of heap_t may contain many arenas! */
	struct arena_t* next; /**< next points to the next arena in the heap. If this is the final arena in the heap, next is equal to null. */
} arena_t;

/**
 * \brief A structure to define the memory leak logger.
 * 
 * The struct logger_t is used to define the contents of the memory leak logger, implemented to fulfill the requirements of Homework 1 of the Game Architecture class. 
 * This structure contains pointers to become a self-contained doubly linked list, as well as functionality to store backtrace entries and the address of the memory allocation being tracked.
 */
typedef struct logger_t
{
	struct logger_t* next; /**< next points to the following logger_t in the list of memory logs. */
	struct logger_t* previous; /**< previous points to the previous logger_t in the list of memory logs. */
	char** names; /**< names stores the backtrace to the memory allocation instance that allocated the block pointed to by address. */
	size_t allocSize; /**< allocSize represents the size of the block allocated at address. */
	void* address; /**< address points to the block of allocated memory that this logger is tracking. */
} logger_t;

/** 
 * \brief A structure to define the engine's heap.
 * 
 * The struct heap_t is used to represent the heap, the space within the engine that manages all memory allocation/de-allocation over the course of the program's execution. 
 * This implementation of heap uses a tlsf implementation written by Matthew Conte to handle all memory operations.
*/
typedef struct heap_t
{
	tlsf_t tlsf; /**< tlsf stores an instance of tlsf_t, used for memory alignment, pool allocation, and memory freeing. */
	size_t grow_increment; /**< grow_increment represents the minimum amount of memory that can be added to the heap in one go - if the heap is out of memory and we try to add an arena to it, with minimum size of grow_increment and maxiumum size of twice whatever the thing we're trying to add is. */
	arena_t* arena; /**< arena points to the first arena_t in this heap. There might be more down the chain! These store the pools of memory that this heap is managing. */
	logger_t* logger; /**< logger points to the first logger_t in a list. Each logger_t stores a backtrace for a specific memory allocation, and if any are not properly freed before the program ends, they will print to console as memory leaks. */
	mutex_t* mutex; /**< mutex points to an instance of mutex_t, an implementation of the mutex concept. This allows us to forbid the simultaneous execution of a given function in multiple threads, ensuring that a function is "thread-safe" - that is, parallel execution over multiple threads will occur in proper order, and not result in unintended behavior. */
} heap_t;

heap_t* heap_create(size_t grow_increment)
{
	heap_t* heap = VirtualAlloc(NULL, sizeof(heap_t) + tlsf_size(),
		MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!heap)
	{
		debug_print(
			k_print_error,
			"OUT OF MEMORY!\n");
		return NULL;
	}

	heap->mutex = mutex_create();
	heap->grow_increment = grow_increment;
	heap->tlsf = tlsf_create(heap + 1);
	heap->arena = NULL;

	return heap;
}

void* heap_alloc(heap_t* heap, size_t size, size_t alignment)
{
	mutex_lock(heap->mutex);

	logger_t* log = VirtualAlloc(NULL, sizeof(logger_t), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	log->names = debug_backtrace();
	log->allocSize = size;
	
	

	void* address = tlsf_memalign(heap->tlsf, alignment, size);
	if (!address)
	{
		size_t arena_size =
			__max(heap->grow_increment, size * 2) +
			sizeof(arena_t);
		arena_t* arena = VirtualAlloc(NULL,
			arena_size + tlsf_pool_overhead(),
			MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (!arena)
		{
			debug_print(
				k_print_error,
				"OUT OF MEMORY!\n");
			return NULL;
		}

		arena->pool = tlsf_add_pool(heap->tlsf, arena + 1, arena_size);

		arena->next = heap->arena;
		heap->arena = arena;

		address = tlsf_memalign(heap->tlsf, alignment, size);
	}
	
	log->address = address;
	log->next = heap->logger;
	if(heap->logger) {
		heap->logger->previous = log;
	}
	heap->logger = log;

	mutex_unlock(heap->mutex);

	return address;
}

void heap_free(heap_t* heap, void* address)
{
	mutex_lock(heap->mutex);
	logger_t* log_ptr = heap->logger;
	if(!log_ptr) {
		debug_print(
				k_print_error,
				"REACHED HEAP FREE WITHOUT ANY MEM LOGS!\n");
		return;
	}
	bool found = false;
	do {
		if (log_ptr->address == address) {
			found = true;
			break;
		}
		log_ptr = log_ptr->next;
	} while (log_ptr);
	if(!found) {
		printf("Free Addr: %p\n", address);
		printf("Log Addr: %p\n", log_ptr->address);
		debug_print(
				k_print_error,
				"COULD NOT FIND MEMORY ADDRESS IN LOGS!\n");
	}

	if(log_ptr->previous && log_ptr->next) {
		log_ptr->previous->next = log_ptr->next;
		log_ptr->next->previous = log_ptr->previous;
	}
	else if(log_ptr->previous) {
		log_ptr->previous->next = NULL;
	}
	else if(log_ptr->next) {
		heap->logger = log_ptr->next;
		log_ptr->next->previous = NULL;
	}
	else{
		heap->logger = NULL;
	}

	VirtualFree(log_ptr, 0, MEM_RELEASE);

	
	tlsf_free(heap->tlsf, address);
	mutex_unlock(heap->mutex);
}

void heap_destroy(heap_t* heap)
{
	logger_t* log_ptr = heap->logger;
	while(log_ptr) {
		printf("Memory leak of size %i bytes with callstack:\n", (int)(log_ptr->allocSize));
		int i = 0;
		bool mainFound = false;
		char main[5] = "main";
		while(log_ptr->names[i]) {
			if (!mainFound) {
				printf("[%d] : %s\n", i, log_ptr->names[i]);
			}
			if (!strcmp(log_ptr->names[i], main)) {
				mainFound = true;
			}
			VirtualFree(log_ptr->names[i], 0, MEM_RELEASE);
			i += 1;
		}
		VirtualFree(log_ptr->names, 0, MEM_RELEASE);
    	logger_t* temp = log_ptr->next;
		VirtualFree(log_ptr, 0, MEM_RELEASE);
		log_ptr = temp;
	}

	tlsf_destroy(heap->tlsf);

	arena_t* arena = heap->arena;
	while (arena)
	{
		arena_t* next = arena->next;
		VirtualFree(arena, 0, MEM_RELEASE);
		arena = next;
	}

	mutex_destroy(heap->mutex);

	VirtualFree(heap, 0, MEM_RELEASE);
}
