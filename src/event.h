#pragma once

#include <stdbool.h>

// Event thread synchronization
/**
 * @file
 * @author Mark Haddleton
 * 
 * Methods for basic event waits and signals.
 */

// Handle to an event.
typedef struct event_t event_t;

// Creates a new event.
/**
 * \brief Creates a generic event.
 * 
 * Uses CreateEvent from synchapi.h in Win32.
 * 
 * \return A generic new event.
 */
event_t* event_create();

// Destroys a previously created event.
/**
 * \brief Destroys the provided event.
 * 
 * \param event The event to be destroyed.
 */
void event_destroy(event_t* event);

// Signals an event.
// All threads waiting on this event will resume.
/**
 * \brief Signals the provided event.
 * 
 * Any threads that are waiting for this event will resume.
 * 
 * \param event The event to be signaled.
 */
void event_signal(event_t* event);

// Waits for an event to be signaled.
/**
 * \brief Waits for the provided event to be signaled.
 * 
 * This forces the current thread to stall until the signal occurs.
 * 
 * \param event The event to wait for.
 */
void event_wait(event_t* event);

// Determines if an event is signaled.
/**
 * \brief Returns true if the provided event is signaled, otherwise false.
 * 
 * \param The event to check the signal status of.
 * \return True if event is signaled, otherwise false.
 */
bool event_is_raised(event_t* event);
