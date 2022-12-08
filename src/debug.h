#pragma once

#include <stdint.h>

// Debugging Support
/**
 * @file
 * @author Mark Haddleton
 * 
 * debug contains functionality to both log and filter debug statements.
 */

// Flags for debug_print().
/**
 * \brief Defines individual bits of the debug print mask for different types of debug statements.
 */
typedef enum debug_print_t
{
	k_print_info = 1 << 0, /**< Info statements. */
	k_print_warning = 1 << 1, /**< Warning statements. */
	k_print_error = 1 << 2, /**< Error statements. */
} debug_print_t;

// Install unhandled exception handler.
// When unhandled exceptions are caught, will log an error and capture a memory dump.
/**
 * \brief Installs the unhandled exception handler.
 * 
 * This function uses AddVectoredExceptionHandler from errhandlingapi.h in Win32.
 */
void debug_install_exception_handler();

// Set mask of which types of prints will actually fire.
// See the debug_print().
/**
 * \brief Sets the print mask, which filters print output by statement type.
 * 
 * \param mask A bit mask of the desired output types.
 */
void debug_set_print_mask(uint32_t mask);

// Log a message to the console.
// Message may be dropped if type is not in the active mask.
// See debug_set_print_mask.
/**
 * \brief Prints a message to the console if its type is in the active print mask.
 * 
 * \sa debug_set_print_mask(mask)
 * 
 * \param type Defines the type of message being sent to console.
 * \param format The string in which to insert values, then print to console.
 * \param ... Variable arguments to insert into the formatted string.
 */
void debug_print(uint32_t type, _Printf_format_string_ const char* format, ...);

// Capture a list of addresses that make up the current function callstack.
// On return, stack contains at most stack_capacity addresses.
// The number of addresses captured is the return value.
/**
 * \brief Captures the callstack to the current location.
 * 
 * The stack capacity is hardcoded to a maximum of 100 frames.
 * 
 * \return The address stack contents.
 */
char** debug_backtrace();
