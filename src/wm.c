#include "wm.h"

#include "debug.h"
#include "heap.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

/**
 * \brief wm_window_t is a structure to store data for the OS-level window.
 * 
 * It contains important info, including the raw window object (HWND), 
 * a back reference to the heap, and other states like the mouse position, quit and focus states, and input masks.
 */
typedef struct wm_window_t
{
	HWND hwnd; /**< hwnd stores the raw OS-level window object. */
	heap_t* heap; /**< heap is a reference to the heap in which the window object is stored. */
	bool quit; /**< quit becomes true if the manager receives a WM_CLOSE flag. Otherwise, quit is false. */
	bool has_focus; /**< has_focus changes state when the manager receives the WM_ACTIVATEAPP flag, becoming true if the window is currently in focus. Otherwise, has_focus is false. This flag is used to enable or disable mouse position updates. */
	uint32_t mouse_mask; /**< mouse_mask is a bit mask that records up/down states for the left, right, and middle mouse buttons. */
	uint32_t key_mask; /**< key_mask is a bit mask that records up/down states for the up, down, left, and right keyboard keys. */
	int mouse_x; /**< mouse_x records the relative x position of the mouse within the window, updating every frame as long as the window is currently in focus. */
	int mouse_y; /**< mouse_y records the relative y position of the mouse within the window, updating every frame as long as the window is currently in focus. */
} wm_window_t;


const struct
{
	int virtual_key;
	int ga_key;
}
k_key_map[] =
{
	{ .virtual_key = VK_LEFT, .ga_key = k_key_left, },
	{ .virtual_key = VK_RIGHT, .ga_key = k_key_right, },
	{ .virtual_key = VK_UP, .ga_key = k_key_up, },
	{ .virtual_key = VK_DOWN, .ga_key = k_key_down, },
};

static LRESULT CALLBACK _window_proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	wm_window_t* win = (wm_window_t*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	if (win)
	{
		switch (uMsg)
		{
		case WM_KEYDOWN:
			for (int i = 0; i < _countof(k_key_map); ++i)
			{
				if (k_key_map[i].virtual_key == wParam)
				{
					win->key_mask |= k_key_map[i].ga_key;
					break;
				}
			}
			break;
		case WM_KEYUP:
			for (int i = 0; i < _countof(k_key_map); ++i)
			{
				if (k_key_map[i].virtual_key == wParam)
				{
					win->key_mask &= ~k_key_map[i].ga_key;
					break;
				}
			}
			break;
		case WM_LBUTTONDOWN:
			win->mouse_mask |= k_mouse_button_left;
			break;
		case WM_LBUTTONUP:
			win->mouse_mask &= ~k_mouse_button_left;
			break;
		case WM_RBUTTONDOWN:
			win->mouse_mask |= k_mouse_button_right;
			break;
		case WM_RBUTTONUP:
			win->mouse_mask &= ~k_mouse_button_right;
			break;
		case WM_MBUTTONDOWN:
			win->mouse_mask |= k_mouse_button_middle;
			break;
		case WM_MBUTTONUP:
			win->mouse_mask &= ~k_mouse_button_middle;
			break;

		case WM_MOUSEMOVE:
			if (win->has_focus)
			{
				// Relative mouse movement in four steps:
				// 1. Get current mouse position (old_cursor).
				// 2. Move mouse back to center of window.
				// 3. Get current mouse position (new_cursor).
				// 4. Compute relative movement (old_cursor - new_cursor).

				POINT old_cursor;
				GetCursorPos(&old_cursor);

				RECT window_rect;
				GetWindowRect(hwnd, &window_rect);
				SetCursorPos(
					(window_rect.left + window_rect.right) / 2,
					(window_rect.top + window_rect.bottom) / 2);

				POINT new_cursor;
				GetCursorPos(&new_cursor);

				win->mouse_x = old_cursor.x - new_cursor.x;
				win->mouse_y = old_cursor.y - new_cursor.y;
			}
			break;

		case WM_ACTIVATEAPP:
			ShowCursor(!wParam);
			win->has_focus = wParam;
			break;

		case WM_CLOSE:
			win->quit = true;
			break;
		}
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

wm_window_t* wm_create(heap_t* heap)
{
	WNDCLASS wc =
	{
		.lpfnWndProc = _window_proc,
		.hInstance = GetModuleHandle(NULL),
		.lpszClassName = L"ga2022 window class",
	};
	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		wc.lpszClassName,
		L"GA 2022",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		wc.hInstance,
		NULL);

	if (!hwnd)
	{
		debug_print(
			k_print_warning,
			"Failed to create window!\n");
		return NULL;
	}

	wm_window_t* win = heap_alloc(heap, sizeof(wm_window_t), 8);
	win->has_focus = false;
	win->hwnd = hwnd;
	win->key_mask = 0;
	win->mouse_mask = 0;
	win->quit = false;
	win->heap = heap;

	SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)win);

	// Windows are created hidden by default, so we
	// need to show it here.
	ShowWindow(hwnd, TRUE);

	return win;
}

bool wm_pump(wm_window_t* window)
{
	MSG msg = { 0 };
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return window->quit;
}

uint32_t wm_get_mouse_mask(wm_window_t* window)
{
	return window->mouse_mask;
}

uint32_t wm_get_key_mask(wm_window_t* window)
{
	return window->key_mask;
}

void wm_get_mouse_move(wm_window_t* window, int* x, int* y)
{
	*x = window->mouse_x;
	*y = window->mouse_y;
}


void wm_destroy(wm_window_t* window)
{
	DestroyWindow(window->hwnd);
	heap_free(window->heap, window);
}

void* wm_get_raw_window(wm_window_t* window)
{
	return window->hwnd;
}
