#include <stdbool.h>
#include <stdint.h>

// Window Manager
// 
// Main object is wm_window_t to represents a single OS-level window.
// Window should be pumped every frame.
// After pumping a window can be queried for user input.

/**
 * @file
 * @author Mark Haddleton
 * 
 * The Window Manager controls the OS-level window: including the initial creation, the update every frame, retrieval of input, and destroying the window.
 */

// Handle to a window.
typedef struct wm_window_t wm_window_t;

typedef struct heap_t heap_t;

// Mouse buttons. See wm_get_mouse_mask().
/**
 * \brief This enum maps the 3 mouse buttons to specific bit shifts within the mouse_mask.
 */
enum
{
	k_mouse_button_left = 1 << 0, /**< The left mouse button. */
	k_mouse_button_right = 1 << 1, /**< The right mouse button. */
	k_mouse_button_middle = 1 << 2, /**< The middle mouse button. */
};

// Keyboard keys. See wm_get_key_mask().
/**
 * \brief This enum maps the 4 arrow keys to specific bit shifts within the key_mask.
 */
enum
{
	k_key_up = 1 << 0, /**< The up arrow key. */
	k_key_down = 1 << 1, /**< The down arrow key. */
	k_key_left = 1 << 2, /**< The left arrow key. */
	k_key_right = 1 << 3, /**< The right arrow key. */
};


// Creates a new window. Must be destroyed with wm_destroy().
// Returns NULL on failure, otherwise a new window.
/**
 * \brief Creates a new window, stored in heap_t heap.
 * 
 * wm_create creates the raw window object in the OS and stores the HWND object in the new wm_window_t, along with accompanying input data and flags.
 * 
 * \param heap The heap to store the wm_window_t in.
 * \return The newly-created wm_window_t.
 */
wm_window_t* wm_create(heap_t* heap);

// Destroy a previously created window.
/**
 * \brief Destroys the provided window.
 * 
 * This destroys the raw object in the OS, then frees the struct from the heap.
 * 
 * \param window The window to be destroyed.
 */
void wm_destroy(wm_window_t* window);

// Pump the messages for a window.
// This will refresh the mouse and key state on the window.
/**
 * \brief Pumps the messages of the provided window, effectively advancing a frame.
 * 
 * \param The window to be pumped.
 * \return True if the program should quit, False otherwise.
 */
bool wm_pump(wm_window_t* window);

// Get a mask of all mouse buttons current held.
/**
 * \brief Returns the mouse mask of the provided window.
 *
 * \param window The window that contains the mouse_mask.
 * \return The mouse mask of the window.
 */
uint32_t wm_get_mouse_mask(wm_window_t* window);

// Get a mask of all keyboard keys current held.
/**
 * \brief Returns the key mask of the provided window.
 *
 * \param window The window that contains the key_mask.
 * \return The key mask of the window.
 */
uint32_t wm_get_key_mask(wm_window_t* window);

// Get relative mouse movement in x and y.
/**
 * \brief Changes the values of the x and y parameters to equal the current mouse position in the window.
 *
 * \param window The window where we're looking for the mouse.
 * \param x A pointer to the x value that will be changed.
 * \param y A pointer to the y value that will be changed.
 */
void wm_get_mouse_move(wm_window_t* window, int* x, int* y);

// Get the raw OS window object.
/**
 * \brief Returns the raw OS window object of the given window.
 * 
 * \param window The window whose object we're retrieving.
 * \return The raw window object.
 */
void* wm_get_raw_window(wm_window_t* window);
