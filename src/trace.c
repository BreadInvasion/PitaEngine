#include "trace.h"
#include "queue.h"
#include "heap.h"
#include <stddef.h>
#include <stdbool.h>
#include <Windows.h>

typedef struct trace_list_t {
	trace_event_t event;
	trace_list_t* next;
} trace_list_t;

typedef struct trace_t {
	bool capturing;
	queue_t* queue;
	trace_list_t* list_head;
	trace_list_t* list_tail;
} trace_t;

typedef struct trace_event_t {
	char* name;
	char* ph;
	int pid;
	DWORD tid;
	int time;
} trace_event_t;

trace_t* trace_create(heap_t* heap, int event_capacity)
{
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), sizeof(bool));
	trace->queue = queue_create(heap, event_capacity);
	trace->capturing = false;
}

void trace_destroy(trace_t* trace)
{

}

void trace_duration_push(trace_t* trace, const char* name)
{
	//Check if capturing
	//Add "B" marked event to queue
	//Add "B" marked event to list
}

void trace_duration_pop(trace_t* trace)
{
	//Check if capturing
	//Pop "B" marked event from queue
	//Add "E" marked event to list
}

void trace_capture_start(trace_t* trace, const char* path)
{
	if (trace == NULL) {
		return;
	}
	trace->capturing = true;
	return;
}

void trace_capture_stop(trace_t* trace)
{
	if (trace == NULL) {
		return;
	}
	trace->capturing = false;
	return;
}
