#include "debug.h"
#include "fs.h"
#include "heap.h"
#include "render.h"
#include "simple_game.h"
#include "timer.h"
#include "wm.h"

#include "cpp_test.h"

/**
 * @file
 * @author Mark Haddleton <haddlm@rpi.edu>
 * 
 * The main file is used to test the engine's functionality and run test cases for specific sections.
 */

/**
 * @brief The main function
 * @author Mark Haddleton
 * 
 * This is the main function of the entire engine. It contains function calls to start: 
 * In-game time, the heap, the file system, the game window, and the renderer. 
 * Once those are in place, the game is initialized, then updated every frame in the core "pump" loop.
 * Upon shutdown, all of the allocated memory blocks are freed and the window is closed.
 */
int main(int argc, const char* argv[])
{
	debug_set_print_mask(k_print_info | k_print_warning | k_print_error);
	debug_install_exception_handler();

	timer_startup();

	cpp_test_function(42);

	heap_t* heap = heap_create(2 * 1024 * 1024);
	fs_t* fs = fs_create(heap, 8);
	wm_window_t* window = wm_create(heap);
	render_t* render = render_create(heap, window);

	simple_game_t* game = simple_game_create(heap, fs, window, render, argc, argv);

	while (!wm_pump(window))
	{
		simple_game_update(game);
	}

	/* XXX: Shutdown render before the game. Render uses game resources. */
	render_destroy(render);

	simple_game_destroy(game);

	wm_destroy(window);
	fs_destroy(fs);
	heap_destroy(heap);

	return 0;
}
