#pragma once

// Atomic operations on 32-bit integers.
/**
 * @file
 * @author Mark Haddleton
 * 
 * atomic contains atomic operations to be performed on 32-bit integers.
 */

// Increment a number atomically.
// Returns the old value of the number.
// Performs the following operation atomically:
//   int old_value = *address; (*address)++; return old_value;
/**
 * \brief Increments the number that address points to, then returns the old value.
 * 
 * \param address The address of the number to be incremented.
 * \return The original value of the number at address.
 */
int atomic_increment(int* address);

// Decrement a number atomically.
// Returns the old value of the number.
// Performs the following operation atomically:
//   int old_value = *address; (*address)--; return old_value;
/**
 * \brief Decrements the number that address points to, then returns the old value.
 *
 * \param address The address of the number to be decremented.
 * \return The original value of the number at address.
 */
int atomic_decrement(int* address);

// Compare two numbers atomically and assign if equal.
// Returns the old value of the number.
// Performs the following operation atomically:
//   int old_value = *address; if (*address == compare) *address = exchange; return old_value;
/**
 * \brief Changes the value at dest to exchange if the value at dest equals compare. Returns the original value at dest.
 * 
 * \param dest The address of the number to be compared and potentially changed.
 * \param compare The number to compare the integer at dest with.
 * \param exchange The value of dest is altered to this number if *dest == compare.
 * \return The original value of dest.
 */
int atomic_compare_and_exchange(int* dest, int compare, int exchange);

// Reads an integer from an address.
// All writes that occurred before the last atomic_store to this address are flushed.
/**
 * \brief Reads an integer from address. All writes before the last atomic_store to address are flushed.
 * 
 * \param address The address to read from.
 * \return The value read from address.
 */
int atomic_load(int* address);

// Writes an integer.
// Paired with an atomic_load, can guarantee ordering and visibility.
/**
 * \brief Writes integer value to address.
 * 
 * This function helps to guarantee ordering and visibility, in combination with the atomic_load(address) function.
 * 
 * \param address The address to store value at.
 * \param value The number to be stored.
 */
void atomic_store(int* address, int value);
