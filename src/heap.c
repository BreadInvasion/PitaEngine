#include "heap.h"

#include "debug.h"
#include "tlsf/tlsf.h"

#include <stddef.h>
#include <stdio.h>
#include <stdbool.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <DbgHelp.h>

typedef struct arena_t
{
	pool_t pool;
	struct arena_t* next;
} arena_t;

typedef struct logger_t
{
	struct logger_t* next;
	struct logger_t* previous;
	char** names;
	size_t allocSize;
	void* address;
} logger_t;

typedef struct heap_t
{
	tlsf_t tlsf;
	size_t grow_increment;
	arena_t* arena;
	logger_t* logger;
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

	heap->grow_increment = grow_increment;
	heap->tlsf = tlsf_create(heap + 1);
	heap->arena = NULL;

	return heap;
}

void* heap_alloc(heap_t* heap, size_t size, size_t alignment)
{
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
	return address;
}

void heap_free(heap_t* heap, void* address)
{
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

	VirtualFree(heap, 0, MEM_RELEASE);
}
