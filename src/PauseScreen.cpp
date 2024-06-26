#include <Windows.h>
#include <shlwapi.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "main.h"
#include "PauseScreen.h"

#include "mod_loader.h"
#include "cave_story.h"
#include "Config.h"
#include <vector>

#define MAX_OPTIONS ((WINDOW_HEIGHT / 20) - 4)	// The maximum number of options we can fit on-screen at once

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

const char* setting_pausetext_paused = "PAUSED";
const char* setting_pausetext_resume = "Resume";
const char* setting_pausetext_reset = "Reset";
const char* setting_pausetext_options = "Options";
const char* setting_pausetext_quit = "Quit";

const char* setting_pausetext_restart_required = "RESTART REQUIRED";
const char* setting_pausetext_menu_options = "OPTIONS";

const char* setting_pausetext_resolution = "Resolution";
const char* setting_pausetext_gamepad_enabled = "Gamepad Enabled";

const char* setting_pausetext_windowsize_a = "Fullscreen";
const char* setting_pausetext_windowsize_b = "Windowed 320x240";
const char* setting_pausetext_windowsize_c = "Windowed 640x480";
const char* setting_pausetext_windowsize_d = "Fullscreen 24-Bit";
const char* setting_pausetext_windowsize_e = "Fullscreen 32-Bit";

const char* setting_pausetext_disabled = "Disabled";
const char* setting_pausetext_enabled = "Enabled";

const char* setting_pausetext_yes = "Yes";
const char* setting_pausetext_no = "No";

const char* setting_pausetext_are_you_sure = "Are you sure?";
const char* setting_pausetext_unsaved_progress = "Unsaved progress will be lost!";

static BOOL restart_required;

static const RECT rcMyChar[4] = {
	{0, 16, 16, 32},
	{16, 16, 32, 32},
	{0, 16, 16, 32},
	{32, 16, 48, 32},
};

Option *options_main = NULL;
Option *options_menu_o = NULL;

size_t num_entries_added_main = 0;
size_t num_entries_added_menu = 0;

Option** GetOptionsMain()
{
	return &options_main;
}

Option** GetOptionsMenu()
{
	return &options_menu_o;
}

size_t* GetNumEntriesAddedMain()
{
	return &num_entries_added_main;
}

size_t* GetNumEntriesAddedMenu()
{
	return &num_entries_added_menu;
}

void add_pause_entry(Option** options_ptr, const char* name, int (*callback)(OptionsMenu* parent_menu, size_t this_option, CallbackAction action), void* user_data, const char* value_string, long value, BOOL disabled, size_t* num_entries_added) {
	// Increase the count of entries added
	(*num_entries_added)++;

	// Reallocate memory for options based on the updated count of entries
	Option* temp = (Option*)realloc(*options_ptr, (*num_entries_added) * sizeof(Option));
	if (temp == NULL) {
		// Handle memory allocation failure
		printf("Memory allocation failed\n");
		exit(EXIT_FAILURE);
	}
	*options_ptr = temp;

	// Assign values to the new entry
	(*options_ptr)[(*num_entries_added) - 1].name = name;
	(*options_ptr)[(*num_entries_added) - 1].callback = callback;
	(*options_ptr)[(*num_entries_added) - 1].user_data = user_data;
	(*options_ptr)[(*num_entries_added) - 1].value_string = value_string;
	(*options_ptr)[(*num_entries_added) - 1].value = value;
	(*options_ptr)[(*num_entries_added) - 1].disabled = disabled;
}

ConfigData pause_conf;

ConfigData* GetConf()
{
	return &pause_conf;
}

std::vector<SaveConfigElementHandler> saveconfigElementHandlers;

void RegisterSaveConfigElement(SaveConfigElementHandler handler)
{
	saveconfigElementHandlers.push_back(handler);
}

void ExecuteSaveConfigElementHandlers()
{
	for (const auto& handler : saveconfigElementHandlers)
	{
		handler();
	}
}

int EnterOptionsMenu(OptionsMenu* options_menu, size_t selected_option)
{
	int scroll = 0;

	unsigned int anime = 0;

	int return_value;

	// Initialise options
	for (size_t i = 0; i < options_menu->total_options; ++i)
		options_menu->options[i].callback(options_menu, i, ACTION_INIT);

	for (;;)
	{
		// Get pressed keys
		GetTrg();

		// Allow unpausing by pressing the pause button only when in the main pause menu (not submenus)
		if (!options_menu->submenu && gKeyTrg & KEY_ESCAPE)
		{
			return_value = CALLBACK_CONTINUE;
			break;
		}

		// Go back one submenu when the 'cancel' button is pressed
		if (gKeyTrg & gKeyCancel)
		{
			return_value = CALLBACK_CONTINUE;
			break;
		}

		// Handling up/down input
		if (gKeyTrg & (gKeyUp | gKeyDown))
		{
			const size_t old_selection = selected_option;

			if (gKeyTrg & gKeyDown)
				if (selected_option++ == options_menu->total_options - 1)
					selected_option = 0;

			if (gKeyTrg & gKeyUp)
				if (selected_option-- == 0)
					selected_option = options_menu->total_options - 1;

			// Update the menu-scrolling, if there are more options than can be fit on the screen
			if (selected_option < old_selection)
				scroll = MAX(0, MIN(scroll, (int)selected_option - 1));

			if (selected_option > old_selection)
				scroll = MIN(MAX(0, (int)options_menu->total_options - MAX_OPTIONS), MAX(scroll, (int)selected_option - (MAX_OPTIONS - 2)));

			PlaySoundObject(1, SOUND_MODE_PLAY);
		}

		// Run option callbacks
		for (size_t i = 0; i < options_menu->total_options; ++i)
		{
			if (!options_menu->options[i].disabled)
			{
				CallbackAction action = ACTION_UPDATE;

				if (i == selected_option)
				{
					if (gKeyTrg & gKeyOk)
						action = ACTION_OK;
					else if (gKeyTrg & gKeyLeft)
						action = ACTION_LEFT;
					else if (gKeyTrg & gKeyRight)
						action = ACTION_RIGHT;
				}

				return_value = options_menu->options[i].callback(options_menu, i, action);

				// If the callback returned something other than CALLBACK_CONTINUE, it's time to exit this submenu
				if (return_value != CALLBACK_CONTINUE)
					break;
			}
		}

		if (return_value != CALLBACK_CONTINUE)
			break;

		// Update Quote animation counter
		if (++anime >= 40)
			anime = 0;

		// Draw screen
		CortBox(&grcFull, 0x000000);

		const size_t visible_options = MIN(MAX_OPTIONS, options_menu->total_options);

		int y = (WINDOW_HEIGHT / 2) - ((visible_options * 20) / 2) - (40 / 2);

		// Draw title
		PutText((WINDOW_WIDTH / 2) - ((strlen(options_menu->title) * 5) / 2), y, options_menu->title, RGB(0xFF, 0xFF, 0xFF));

		// Draw subtitle
		if (options_menu->subtitle != NULL)
			PutText((WINDOW_WIDTH / 2) - ((strlen(options_menu->subtitle) * 5) / 2), y + 14, options_menu->subtitle, RGB(0xFF, 0xFF, 0xFF));

		y += 40;

		for (size_t i = scroll; i < scroll + visible_options; ++i)
		{
			const int x = (WINDOW_WIDTH / 2) + options_menu->x_offset;

			// Draw Quote next to the selected option
			if (i == selected_option)
				PutBitmap3(&grcFull, x - 20, y - 8, &rcMyChar[anime / 10 % 4], SURFACE_ID_MY_CHAR);

			unsigned long option_colour = options_menu->options[i].disabled ? RGB(0x80, 0x80, 0x80) : RGB(0xFF, 0xFF, 0xFF);

			// Draw option name
			PutText(x, y - (9 / 2), options_menu->options[i].name, option_colour);

			// Draw option value, if it has one
			if (options_menu->options[i].value_string != NULL)
				PutText(x + 100, y - (9 / 2), options_menu->options[i].value_string, option_colour);

			y += 20;
		}

		PutFramePerSecound();

		if (!Flip_SystemTask(ghWnd))
		{
			// Quit if window is closed
			return_value = CALLBACK_EXIT;
			break;
		}
	}

	// Deinitialise options
	for (size_t i = 0; i < options_menu->total_options; ++i)
		options_menu->options[i].callback(options_menu, i, ACTION_DEINIT);

	return return_value;
}

//////////////////
// Options menu //
//////////////////

static int Callback_Resolution(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	ConfigData* conf = (ConfigData*)parent_menu->options[this_option].user_data;

	const char* strings[] = { setting_pausetext_windowsize_a, setting_pausetext_windowsize_b, setting_pausetext_windowsize_c, setting_pausetext_windowsize_d, setting_pausetext_windowsize_e };

	switch (action)
	{
		case ACTION_INIT:
			parent_menu->options[this_option].value = conf->display_mode;
			parent_menu->options[this_option].value_string = strings[conf->display_mode];
			break;

		case ACTION_DEINIT:
			conf->display_mode = parent_menu->options[this_option].value;
			break;

		case ACTION_OK:
		case ACTION_LEFT:
		case ACTION_RIGHT:
			restart_required = TRUE;
			parent_menu->subtitle = setting_pausetext_restart_required;

			if (action == ACTION_LEFT)
			{
				// Decrement value (with wrapping)
				if (--parent_menu->options[this_option].value < 0)
					parent_menu->options[this_option].value = (sizeof(strings) / sizeof(strings[0])) - 1;
			}
			else
			{
				// Increment value (with wrapping)
				if (++parent_menu->options[this_option].value > (sizeof(strings) / sizeof(strings[0])) - 1)
					parent_menu->options[this_option].value = 0;
			}

			PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

			parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
			break;

		case ACTION_UPDATE:
			break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_ControllerEnabled(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	ConfigData* conf = (ConfigData*)parent_menu->options[this_option].user_data;

	const char* strings[] = { setting_pausetext_disabled, setting_pausetext_enabled };

	switch (action)
	{
	case ACTION_INIT:
		parent_menu->options[this_option].value = conf->bJoystick;
		parent_menu->options[this_option].value_string = strings[conf->bJoystick];
		break;

	case ACTION_DEINIT:
		conf->bJoystick = parent_menu->options[this_option].value;
		break;

	case ACTION_OK:
	case ACTION_LEFT:
	case ACTION_RIGHT:
		// Increment value (with wrapping)
		parent_menu->options[this_option].value = (parent_menu->options[this_option].value + 1) % (sizeof(strings) / sizeof(strings[0]));

		gbUseJoystick = parent_menu->options[this_option].value;

		PlaySoundObject(SND_SWITCH_WEAPON, SOUND_MODE_PLAY);

		parent_menu->options[this_option].value_string = strings[parent_menu->options[this_option].value];
		break;

	case ACTION_UPDATE:
		break;
	}

	return CALLBACK_CONTINUE;
}

static int Callback_Options(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	OptionsMenu options_menu = {
		setting_pausetext_menu_options,
		restart_required ? setting_pausetext_restart_required : NULL,
		options_menu_o,
		num_entries_added_menu,
		-70,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	const int return_value = EnterOptionsMenu(&options_menu, 0);

	PlaySoundObject(5, SOUND_MODE_PLAY);

	SaveConfigData(GetConf());
	ExecuteSaveConfigElementHandlers(); // execute save config handlers for modders to save their configs at the same time as this dll

	return return_value;
}

////////////////
// Pause menu //
////////////////

static int PromptAreYouSure(void)
{
	struct FunctionHolder
	{
		static int Callback_Yes(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
		{
			(void)parent_menu;
			(void)this_option;

			if (action != ACTION_OK)
				return CALLBACK_CONTINUE;

			return 1;	// Yes
		}

		static int Callback_No(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
		{
			(void)parent_menu;
			(void)this_option;

			if (action != ACTION_OK)
				return CALLBACK_CONTINUE;

			return 0;	// No
		}
	};

	Option options[] = {
		{setting_pausetext_yes, FunctionHolder::Callback_Yes, NULL, NULL, 0, FALSE},
		{setting_pausetext_no, FunctionHolder::Callback_No, NULL, NULL, 0, FALSE}
	};

	OptionsMenu options_menu = {
		setting_pausetext_are_you_sure,
		setting_pausetext_unsaved_progress,
		options,
		sizeof(options) / sizeof(options[0]),
		-10,
		TRUE
	};

	PlaySoundObject(5, SOUND_MODE_PLAY);

	int return_value = EnterOptionsMenu(&options_menu, 1);

	PlaySoundObject(18, SOUND_MODE_PLAY);

	return return_value;
}

static int Callback_Resume(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	PlaySoundObject(18, SOUND_MODE_PLAY);
	return enum_ESCRETURN_continue;
}

static int Callback_Reset(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	int return_value = PromptAreYouSure();

	switch (return_value)
	{
	case 0:
		return_value = CALLBACK_CONTINUE;	// Go back to previous menu
		break;

	case 1:
		return_value = CALLBACK_RESET;	// Restart game
		break;
	}

	return return_value;
}

static int Callback_Quit(OptionsMenu* parent_menu, size_t this_option, CallbackAction action)
{
	(void)parent_menu;

	if (action != ACTION_OK)
		return CALLBACK_CONTINUE;

	int return_value = PromptAreYouSure();

	switch (return_value)
	{
	case 0:
		return_value = CALLBACK_CONTINUE;	// Go back to previous menu
		break;

	case 1:
		return_value = CALLBACK_EXIT;	// Exit game
		break;
	}

	return return_value;
}

void InitPauseScreen()
{
	// Make the options match the configuration data
	if (!LoadConfigData(GetConf()))
		DefaultConfigData(GetConf());

	add_pause_entry(&options_main, setting_pausetext_resume, Callback_Resume, NULL, NULL, 0, FALSE, &num_entries_added_main);
	add_pause_entry(&options_main, setting_pausetext_reset, Callback_Reset, NULL, NULL, 0, FALSE, &num_entries_added_main);
	add_pause_entry(&options_main, setting_pausetext_options, Callback_Options, NULL, NULL, 0, FALSE, &num_entries_added_main);
	add_pause_entry(&options_main, setting_pausetext_quit, Callback_Quit, NULL, NULL, 0, FALSE, &num_entries_added_main);
	add_pause_entry(&options_menu_o, setting_pausetext_resolution, Callback_Resolution, GetConf(), NULL, 0, FALSE, &num_entries_added_menu);
	add_pause_entry(&options_menu_o, setting_pausetext_gamepad_enabled, Callback_ControllerEnabled, GetConf(), NULL, 0, FALSE, &num_entries_added_menu);
}

int Call_Pause(void)
{
	OptionsMenu options_menu = {
		setting_pausetext_paused,
		NULL,
		options_main,
		num_entries_added_main,
		-14,
		FALSE
	};

	int return_value = EnterOptionsMenu(&options_menu, 0);

	// Filter internal return values to something Cave Story can understand
	switch (return_value)
	{
	case CALLBACK_CONTINUE:
		return_value = enum_ESCRETURN_continue;
		break;

	case CALLBACK_RESET:
		return_value = enum_ESCRETURN_restart;
		break;

	case CALLBACK_EXIT:
		return_value = enum_ESCRETURN_exit;
		break;
	}

	gKeyTrg = gKey = 0;	// Avoid input-ghosting

	return return_value;
}
