// Author: Sean Pesce
// Original d3d9.dll wrapper base by Michael Koch

#pragma once

#ifndef _SP_D3D9_OVERLAY_TEMPLATE_H_
	#define _SP_D3D9_OVERLAY_TEMPLATE_H_

#include "SP_IO.hpp"


//////////////////////// WRAPPER DLL FUNCTIONS ////////////////////////
// Exported function
IDirect3D9* WINAPI Direct3DCreate9 (UINT SDKVersion);
// Regular functions
void InitInstance(HANDLE hModule);
void InitSettings(); // Parses settings file (.ini) for intialization settings
void ExitInstance(void); // Unloads DLL when exiting
void LoadOriginalDll(void); // Loads the original d3d9.dll from the system directory


//////////////////////// CONSTANTS & ENUMS ////////////////////////
// Settings file data:
#define _SP_DS_SETTINGS_FILE_ ".\\d3d9_Mod.ini"	// Settings file name
//	Settings file sections
#define _SP_DS_SETTINGS_SECTION_KEYBINDS_ "Keybinds"
#define _SP_DS_SETTINGS_SECTION_PREFS_ "Preferences"
#define _SP_DS_SETTINGS_SECTION_ADV_SETTINGS_ "Advanced Settings"
#define _SP_DS_SETTINGS_SECTION_DEV_KEYBINDS_ "Developer Keybinds"
#define _SP_DS_SETTINGS_SECTION_DEV_PREFS_ "Developer Preferences"
//	Advanced settings section keys
#define _SP_DS_DLL_CHAIN_KEY_ "d3d9Chain"
#define _SP_DS_DLL_GENERIC_KEY_ "GenericDLL"
#define _SP_DS_DSPW_ADJUSTMENT_KEY_ "DspwOverlayAdjustment"
//	Keybinds section keys
#define _SP_DS_HOTKEY_TOGGLE_OL_TXT_KEY_ "ToggleOverlay"
#define _SP_DS_HOTKEY_NEXT_OL_TXT_POS_KEY_ "ChangeOverlayTextPosition"
#define _SP_DS_HOTKEY_NEXT_OL_TXT_STYLE_KEY_ "ChangeOverlayTextStyle"
#define _SP_DS_HOTKEY_TOGGLE_TEXT_FEED_INFO_BAR_KEY_ "ToggleInfoBar"
//	User preferences section keys
#define _SP_DS_OL_TXT_ENABLED_KEY_ "EnableOverlay"
#define _SP_DS_OL_TXT_SIZE_KEY_ "OverlayTextSize"
#define _SP_DS_OL_TXT_HORIZONTAL_POS_KEY_ "OverlayTextHorizontalPosition"
#define _SP_DS_OL_TXT_VERTICAL_POS_KEY_ "OverlayTextVerticalPosition"
#define _SP_DS_OL_TXT_STYLE_KEY_ "OverlayTextStyle"
#define _SP_DS_OL_TXT_AUDIO_ENABLED_KEY_ "EnableAudioFeedback"
#define _SP_DS_OL_TXT_ENABLE_FPS_KEY_ "DisplayFPS"
#define _SP_DS_OL_TXT_ENABLE_TIME_KEY_ "DisplayTime"
#define _SP_DS_OL_TXT_ENABLE_DATE_KEY_ "DisplayDate"
//	Developer keybinds section keys
#define _SP_DS_HOTKEY_PRINT_OL_TXT_TEST_MSG_KEY_ "PrintTestMessage"
#define _SP_DS_HOTKEY_TOGGLE_AUDIO_FEEDBACK_KEY_ "ToggleAudioFeedback"
#define _SP_DS_HOTKEY_TOGGLE_VERBOSE_OUTPUT_KEY_ "ToggleVerboseOutput"
#define _SP_DS_HOTKEY_INCREASE_TXT_SIZE_KEY_ "IncreaseTextSize"
#define _SP_DS_HOTKEY_DECREASE_TXT_SIZE_KEY_ "DecreaseTextSize"
#define _SP_DS_HOTKEY_RESET_TXT_SIZE_KEY_ "ResetTextSize"
//	Developer preferences section keys
#define _SP_DS_OL_TXT_VERBOSE_OUTPUT_ENABLED_KEY_ "EnableVerboseOutput"

// Enumerator whose values specify whether a toggleable setting is enabled or disabled:
enum SP_OL_TXT_ENABLED_ENUM {
	OL_TXT_DISABLED,
	OL_TXT_ENABLED
};
// Enumerator whose values specify horizontal and vertical positions of overlay feed text:
enum SP_OL_TXT_POS_ENUM {
	OL_TXT_POS_LEFT,
	OL_TXT_POS_HCENTER,
	OL_TXT_POS_RIGHT,
	OL_TXT_POS_TOP,
	OL_TXT_POS_VCENTER,
	OL_TXT_POS_BOTTOM
};

// Acceptable values that a user can specify for text position in the settings file:
const char *SP_OL_TXT_POS_VALS[6] = { "LEFT", "CENTER", "RIGHT", "TOP", "CENTER", "BOTTOM" }; // If specified string isn't one of these values, the default value is used
// Acceptable values that a user can specify for text style in the settings file:
const char *SP_OL_TXT_STYLE_VALS[3] = { "OUTLINE", "SHADOW", "PLAIN" }; // If specified string isn't one of these values, the default value is used

// Default initialization setting values:
#define _SP_DS_DEFAULT_VAL_OL_TXT_ENABLED_ OL_TXT_ENABLED
#define _SP_DS_DEFAULT_VAL_OL_TXT_AUDIO_ENABLED_ OL_TXT_DISABLED
#define _SP_DS_DEFAULT_VAL_OL_TXT_VERBOSE_OUTPUT_ENABLED_ OL_TXT_DISABLED
#define _SP_DS_DEFAULT_VAL_OL_TXT_HORIZONTAL_POS_ SP_OL_TXT_POS_VALS[OL_TXT_POS_HCENTER]
#define _SP_DS_DEFAULT_VAL_OL_TXT_VERTICAL_POS_ SP_OL_TXT_POS_VALS[OL_TXT_POS_BOTTOM]
#define _SP_DS_DEFAULT_VAL_OL_TXT_STYLE_ SP_OL_TXT_STYLE_VALS[SP_D3D9O_OUTLINED_TEXT]


//////////////////////// VARIABLES & DATA ////////////////////////

// Initialization data
HANDLE mod_thread;		// Mod thread handle
DWORD mod_thread_id;	// Mod thread ID
std::string d3d9_dll_chain; // Filename of the d3d9.dll wrapper to chain with (if any)
bool d3d9_dll_chain_failed; // Indicated that a d3d9.dll wrapper chain was specified, but failed to load
unsigned int generic_dll_count; // Number of generic DLLs loaded at runtime

// User preferences
bool user_pref_overlay_text_feed_enabled;
bool user_pref_audio_feedback_enabled;
bool user_pref_verbose_output_enabled;
int user_pref_overlay_text_size;
DWORD user_pref_overlay_text_pos;
SP_D3D9O_TEXT_FEED_STYLE_ENUM user_pref_overlay_text_style;
int user_pref_show_text_feed_info_bar;
// Dark Souls PvP Watchdog Settings
// (These values will be used to adjust the overlay to avoid clipping with the DSPW overlay)
int dspw_pref_font_size;
int user_pref_dspw_ol_offset;
// Mod variables & data
SHORT key_state[256];	// Buffer for async key states
extern bool mod_loop_enabled; // Controls whether the main loop for the mod is enabled/disabled


//////////////////////// MOD FUNCTION PROTOTYPES ////////////////////////
DWORD WINAPI init_mod_thread(LPVOID lpParam); // Determines whether mod is enabled and calls the main loop for the mod
void get_keybinds(); // Reads in configurable keybind values as specified in the settings file (.ini)
void get_user_preferences(); // Reads in user preferences as specified in the settings file (.ini)
HINSTANCE load_dll_from_settings_file(const char *file_name, const char *section, const char *key, char *buffer, unsigned int buffer_size); // Loads a single DLL specified by the given settings file, section, and key parameters
unsigned int load_generic_dlls_from_settings_file(const char *file_name, const char *section, const char *base_key); // Load as many generic DLLs (not wrappers) as are specified in the settings file (key numbers must be consecutive)
int get_dspw_font_size(); // Reads the PvP Watchdog settings file (DSPWSteam.ini) to obtain the DSPW font size in case user wants to adjust this overlay to avoid clipping with the PvP Watchdog overlay


#endif // _SP_D3D9_OVERLAY_TEMPLATE_H_