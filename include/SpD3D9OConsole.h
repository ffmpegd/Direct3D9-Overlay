// Author: Sean Pesce

#pragma once


#ifndef _SP_D3D9O_CONSOLE_H_
	#define _SP_D3D9O_CONSOLE_H_

#include "stdafx.h"
#include "SpD3D9.h"

#include "D3DFont.h"


#ifndef hotkey_is_down
	#define hotkey_is_down(hotkey) (hotkey != 0 && (key_state[hotkey] & _SP_KEY_DOWN_))
#endif

#define _CLOSE_CONSOLE_KEY_ VK_ESCAPE // Escape key
#define _SP_D3D9O_C_DEFAULT_OUTPUT_LOG_CAPACITY_ 100
#define _SP_D3D9O_C_DEFAULT_COMMAND_LOG_CAPACITY_ 20


extern SpD3D9OInputHandler *gl_input_handler;


class SpD3D9OConsole
{
public:
	SpD3D9Overlay *overlay = NULL; // D3D9 overlay that this console belongs to

	std::string prompt = ">";
	std::string command = ""; // Current command being typed

	CD3DFont *font = NULL;
	int font_height = _SP_D3D9O_TF_DEFAULT_FONT_HEIGHT_;
	D3DXCOLOR font_color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);	// White
	std::string font_family = "Courier New";
	D3DXCOLOR background_color = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f); // Black

	std::vector<std::string> command_log; // Log of console commands previously entered
	std::list<std::string> output_log; // Log of console commands previously entered and their resulting outputs
	unsigned int output_log_displayed_lines = 30; // Number of lines of previous output to display
	unsigned int output_log_capacity = _SP_D3D9O_C_DEFAULT_OUTPUT_LOG_CAPACITY_; // Number of lines of output to keep in memory (oldest are deleted when max is hit)
	unsigned int command_log_capacity = _SP_D3D9O_C_DEFAULT_COMMAND_LOG_CAPACITY_; // Number of console commands to keep logged (oldest are deleted when max is hit)

	unsigned int caret_position = 0; // Position of cursor in current command
	unsigned int command_log_position = 0; // Used to obtain previous commands with the up/down keys

	// Constructor/destructor
	SpD3D9OConsole(SpD3D9Overlay *new_overlay);
	~SpD3D9OConsole();

	void SpD3D9OConsole::get_input(); // Called from main overlay input loop
	bool SpD3D9OConsole::toggle();
	bool SpD3D9OConsole::is_open();
	void SpD3D9OConsole::draw();
	void SpD3D9OConsole::print(const char *new_message); // Prints text to output log
	#ifdef _SP_USE_DINPUT8_CREATE_DEVICE_INPUT_
		void SpD3D9OConsole::handle_key_event(DIDEVICEOBJECTDATA *event);
	#else // !_SP_USE_DINPUT8_CREATE_DEVICE_INPUT_
		void SpD3D9OConsole::handle_key_press(WPARAM wParam);
		void SpD3D9OConsole::handle_text_input(WPARAM wParam);
	#endif // _SP_USE_DINPUT8_CREATE_DEVICE_INPUT_

private:
	bool show_caret = false;
	char caret = '_'; // 
	int caret_blink_delay = 500;  // Speed at which the cursor blinks, in milliseconds
	DWORD next_caret_blink = 0; // Time of next caret toggle
	std::string output_string = "";
};


int open_console(); // Opens console


#endif // _SP_D3D9O_CONSOLE_H_