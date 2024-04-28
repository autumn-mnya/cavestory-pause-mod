#pragma once

#define MAX_OPTIONS_TABLE 0x200

extern const char* setting_pausetext_paused;
extern const char* setting_pausetext_resume;
extern const char* setting_pausetext_reset;
extern const char* setting_pausetext_options;
extern const char* setting_pausetext_quit;

extern const char* setting_pausetext_restart_required;
extern const char* setting_pausetext_menu_options;

extern const char* setting_pausetext_resolution;
extern const char* setting_pausetext_gamepad_enabled;

extern const char* setting_pausetext_windowsize_a;
extern const char* setting_pausetext_windowsize_b;
extern const char* setting_pausetext_windowsize_c;
extern const char* setting_pausetext_windowsize_d;
extern const char* setting_pausetext_windowsize_e;

extern const char* setting_pausetext_disabled;
extern const char* setting_pausetext_enabled;

extern const char* setting_pausetext_yes;
extern const char* setting_pausetext_no;

extern const char* setting_pausetext_are_you_sure;
extern const char* setting_pausetext_unsaved_progress;

enum
{
	CALLBACK_CONTINUE = -1,
	CALLBACK_PREVIOUS_MENU = -2,
	CALLBACK_RESET = -3,
	CALLBACK_EXIT = -4,
};

typedef enum CallbackAction
{
	ACTION_INIT,
	ACTION_DEINIT,
	ACTION_UPDATE,
	ACTION_OK,
	ACTION_LEFT,
	ACTION_RIGHT
} CallbackAction;

typedef struct Option
{
	const char* name;
	int (*callback)(struct OptionsMenu* parent_menu, size_t this_option, CallbackAction action);
	void* user_data;
	const char* value_string;
	long value;
	BOOL disabled;
} Option;

typedef struct OptionsMenu
{
	const char* title;
	const char* subtitle;
	struct Option* options;
	size_t total_options;
	int x_offset;
	BOOL submenu;
} OptionsMenu;

extern Option* options_main;
extern size_t num_entries_added_main;
extern size_t num_entries_added_menu;

extern "C" __declspec(dllexport) Option** GetOptionsMain();
extern "C" __declspec(dllexport) Option** GetOptionsMenu();
extern "C" __declspec(dllexport) size_t* GetNumEntriesAddedMain();
extern "C" __declspec(dllexport) size_t* GetNumEntriesAddedMenu();
extern "C" __declspec(dllexport) void add_pause_entry(Option** options_ptr, const char* name, int (*callback)(OptionsMenu* parent_menu, size_t this_option, CallbackAction action), void* user_data, const char* value_string, long value, BOOL disabled, size_t* num_entries_added);
extern "C" __declspec(dllexport) int EnterOptionsMenu(OptionsMenu * options_menu, size_t selected_option);

void InitPauseScreen();
int Call_Pause(void);