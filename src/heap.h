#pragma once


/**
 * @file
 * @author Mark Haddleton
 * 
 * The heap is a collective memory handling location, in which we can assign both "arenas" for separate pools of memory, 
 * and allocate or destroy specific blocks of memory for use elsewhere in the engine.
 */

#include <stdlib.h>

// Heap Memory Manager
// 
// Main object, heap_t, represents a dynamic memory heap.
// Once created, memory can be allocated and free from the heap.

// Handle to a heap.
typedef struct heap_t heap_t;

// Creates a new memory heap.
// The grow increment is the default size with which the heap grows.
// Should be a multiple of OS page size.
/**
 * \brief Create and return an empty heap with minimum grow increment grow_increment.
 *
 * \param grow_increment The minimum number of bytes that a newly-created arena may contain.
 * \return The newly-created heap.
 */
heap_t* heap_create(size_t grow_increment);

// Destroy a previously created heap.
/**
 * \brief Destroys heap_t heap.
 *
 * \param heap The heap to be destroyed.
 */
void heap_destroy(heap_t* heap);

// Allocate memory from a heap.
/**
 * \brief Allocates a number of bytes equal to size in heap_t heap, aligned an alignment number of bytes from the last block.
 *
 * \param heap The heap to which the new memory will be allocated.
 * \param size The size of the new block.
 * \param alignment The number of bytes to "pad" the new block of memory with, in order to assure high-speed access.
 * \return The memory address of the newly-allocated block.
 */
void* heap_alloc(heap_t* heap, size_t size, size_t alignment);

// Free memory previously allocated from a heap.
/**
 * \brief Frees the memory block at address in heap_t heap.
 *
 * \param heap The heap in which the block to be freed is located.
 * \param address The address of the memory block to be freed.
 */
void heap_free(heap_t* heap, void* address);
