#include "trace.h"
#include "queue.h"
#include "heap.h"
#include "timer.h"
#include <stddef.h>
#include <stdbool.h>
#include <Windows.h>
#include <stdio.h>

// Trace structure
// Contains boolean that controls capturing status, and all relevant storage structures, as well as a pointer to the heap
typedef struct trace_t {
	bool capturing;
	heap_t* heap;
	queue_t* queue;
	trace_event_t* list_head;
	trace_event_t* list_tail;
	const char* path;
} trace_t;

// Event Structure
// Contains event info, and a pointer to the next event sequentially (if any)
typedef struct trace_event_t {
	const char* name;
	char ph;
	DWORD pid;
	DWORD tid;
	uint32_t ts;
	trace_event_t* next;
} trace_event_t;

// Create Trace
// Allocate memory on the heap for the new trace object, and create the empty queue
trace_t* trace_create(heap_t* heap, int event_capacity)
{
	trace_t* trace = heap_alloc(heap, sizeof(trace_t), sizeof(void*));
	trace->queue = queue_create(heap, event_capacity);
	trace->heap = heap;
	trace->capturing = false;
	return trace;
}

// Destroy Trace
// Destroy all event objects as well as the queue, then free the space allocated to the trace
void trace_destroy(trace_t* trace)
{
	trace_event_t* l = trace->list_head;
	while (l) {
		trace_event_t* n = l->next;
		heap_free(trace->heap, l);
		l = n;
	}
	queue_destroy(trace->queue);
	heap_free(trace->heap, trace);
}

// Push Duration to Trace
// Make sure trace is capturing. If so, create new event with a given name, the current system time in milliseconds, and relevant thread/process ids. Add it to the queue and the end of the list pointer series.
void trace_duration_push(trace_t* trace, const char* name)
{
	if (!trace->capturing) {
		return;
	}
	trace_event_t* new_event = heap_alloc(trace->heap, sizeof(trace_event_t), sizeof(void*));
	new_event->ts = timer_ticks_to_ms(timer_get_ticks());
	new_event->name = name;
	new_event->ph = 'B';
	new_event->pid = GetCurrentProcessId();
	new_event->tid = GetCurrentThreadId();
	new_event->next = NULL;
	//Add "B" marked event to queue
	queue_push(trace->queue, new_event);
	//Add "B" marked event to list
	if (!(trace->list_tail)) {
		trace->list_head = new_event;
		trace->list_tail = new_event;
	}
	else {
		trace->list_tail->next = new_event;
		trace->list_tail = new_event;
	}
}

// Pop Duration from Trace
// Make sure trace is capturing. If so, pop the queue and create a new event with the same name but "E" ph instead of "B", and the new current system time. Add said event to the end of the list pointer series.
void trace_duration_pop(trace_t* trace)
{
	if (!trace->capturing) {
		return;
	}
	//Pop "B" marked event from queue
	trace_event_t* event = queue_pop(trace->queue);
	//Add "E" marked event to list
	trace_event_t* new_event = heap_alloc(trace->heap, sizeof(trace_event_t), sizeof(void*));
	new_event->name = event->name;
	new_event->ph = 'E';
	new_event->pid = GetCurrentProcessId();
	new_event->tid = GetCurrentThreadId();
	new_event->ts = timer_ticks_to_ms(timer_get_ticks());
	new_event->next = NULL;
	trace->list_tail->next = new_event;
	trace->list_tail = new_event;
}

// Start Trace Capture
// Set capturing to true, and set the path value.
void trace_capture_start(trace_t* trace, const char* path)
{
	if (trace == NULL) {
		return;
	}
	trace->capturing = true;
	trace->path = path;
	return;
}

// Stop Trace Capture
// Set capturing to false, then write the contents of trace->list_head and subsequent events to the file, formatted into a specific format. At the bottom of the while loop is a case to add commas after every event except for the final one.
void trace_capture_stop(trace_t* trace)
{
	if (trace == NULL) {
		return;
	}
	trace->capturing = false;
	wchar_t wide_path[1024];
	MultiByteToWideChar(CP_UTF8, 0, trace->path, -1, wide_path, sizeof(wide_path));
	HANDLE handle = CreateFile(wide_path, GENERIC_WRITE, FILE_SHARE_WRITE, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	char str[2048] = "{\n\t\"displayTimeUnit\": \"ns\", \"traceEvents\": [\n";
	WriteFile(handle, (LPVOID)str, (DWORD)strlen(str), NULL, NULL);
	trace_event_t* l = trace->list_head;
	while (l) {
		char strv[2048];
		snprintf(strv, sizeof(strv), "\t\t{\"name\":\"%s\",\"ph\":\"%c\",\"pid\":%d,\"tid\":\"%lu\",\"ts\":\"%d\"}", l->name, l->ph, l->pid, l->tid, l->ts);
		WriteFile(handle, (LPVOID)strv, (DWORD)strlen(strv), NULL, NULL);
		l = l->next;
		if (l) {
			char stre[5] = ",\n";
			WriteFile(handle, (LPVOID)stre, (DWORD)strlen(stre), NULL, NULL);
		}
		else {
			char stre[5] = "\n";
			WriteFile(handle, (LPVOID)stre, (DWORD)strlen(stre), NULL, NULL);
		}
	}
	char strcf[2048] = "\t]\n}";
	WriteFile(handle, (LPVOID)strcf, (DWORD)strlen(strcf), NULL, NULL);
	return;
}
