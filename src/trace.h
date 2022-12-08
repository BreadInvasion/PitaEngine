#pragma once

/**
 * @file
 * @author Mark Haddleton
 *
 * Traces are used to track the length of time that occurs between two points in the code. 
 * They can also be used to see the difference in starting times between multiple events, 
 * or they can be nested as shown in Homework 3. Regardless of the specific application, tracing is a very valuable tool.
 */

typedef struct heap_t heap_t;

typedef struct trace_t trace_t;

typedef struct trace_event_t trace_event_t;

// Creates a CPU performance tracing system.
// Event capacity is the maximum number of durations that can be traced.
/**
 * \brief Creates a new trace system.
 * 
 * Note: The trace does not immediately begin capturing upon creation.
 * 
 * \param heap The heap in which to create the trace.
 * \param event_capacity The maximum number of events that may be tracked simultaneously.
 * \return The newly created trace instance.
 */
trace_t* trace_create(heap_t* heap, int event_capacity);

// Destroys a CPU performance tracing system.
/**
 * \brief Destroys the provided trace system.
 * 
 * \param trace The trace system to destroy.
 */
void trace_destroy(trace_t* trace);

// Begin tracing a named duration on the current thread.
// It is okay to nest multiple durations at once.
/**
 * \brief Begins tracing an event called name on the current thread.
 * 
 * Nesting multiple durations is permitted.
 * 
 * \param trace The tracing system handling the event.
 * \param name The name of the new event.
 */
void trace_duration_push(trace_t* trace, const char* name);

// End tracing the currently active duration on the current thread.
/**
 * \brief Stops tracing an event called name on the current thread.
 *
 * Nesting multiple durations is permitted.
 *
 * \param trace The tracing system handling the event.
 * \param name The name of the event being ended.
 */
void trace_duration_pop(trace_t* trace);

// Start recording trace events.
// A Chrome trace file will be written to path.
/**
 * \brief Begins capturing events, and stores the output file path.
 * 
 * \param trace The trace to be activated.
 * \param path The file path to which the activated capture session should be written.
 */
void trace_capture_start(trace_t* trace, const char* path);

// Stop recording trace events.
/**
 * \brief Stops capturing events, and writes all events to a Chrome tracing file at trace->path.
 * 
 * \param trace The trace to be stopped and formatted.
 */
void trace_capture_stop(trace_t* trace);
