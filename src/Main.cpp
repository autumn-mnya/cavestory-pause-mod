#include <windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include "mod_loader.h"
#include "cave_story.h"
#include "PauseScreen.h"

#include "AutPI.h"

void InitSettings()
{
	setting_pausetext_paused = ModLoader_GetSettingString("Paused Text", "PAUSED");
	setting_pausetext_resume = ModLoader_GetSettingString("Resume Text", "Resume");
	setting_pausetext_reset = ModLoader_GetSettingString("Reset Text", "Reset");
	setting_pausetext_options = ModLoader_GetSettingString("Options Text", "Options");
	setting_pausetext_quit = ModLoader_GetSettingString("Quit Text", "Quit");
	setting_pausetext_restart_required = ModLoader_GetSettingString("Restart Required Text", "RESTART REQUIRED");
	setting_pausetext_menu_options = ModLoader_GetSettingString("Options Menu Text", "OPTIONS");

	setting_pausetext_resolution = ModLoader_GetSettingString("Resolution Text", "Resolution");
	setting_pausetext_gamepad_enabled = ModLoader_GetSettingString("Gamepad Text", "Gamepad Enabled");
	setting_pausetext_windowsize_a = ModLoader_GetSettingString("Fullscreen Text", "Fullscreen");
	setting_pausetext_windowsize_b = ModLoader_GetSettingString("Windowed 1 Text", "Windowed 320x240");
	setting_pausetext_windowsize_c = ModLoader_GetSettingString("Windowed 2 Text", "Windowed 640x480");
	setting_pausetext_windowsize_d = ModLoader_GetSettingString("Fullscreen 24bit Text", "Fullscreen 24-Bit");
	setting_pausetext_windowsize_e = ModLoader_GetSettingString("Fullscreen 32bit Text", "Fullscreen 32-Bit");
	setting_pausetext_disabled = ModLoader_GetSettingString("Disabled Text", "Disabled");
	setting_pausetext_enabled = ModLoader_GetSettingString("Enabled Text", "Enabled");
	setting_pausetext_yes = ModLoader_GetSettingString("Yes Text", "Yes");
	setting_pausetext_no = ModLoader_GetSettingString("No Text", "No");
	setting_pausetext_are_you_sure = ModLoader_GetSettingString("Confirm Text", "Are you sure?");
	setting_pausetext_unsaved_progress = ModLoader_GetSettingString("Progress Text", "Unsaved progress will be lost!");
}

void InitMod(void)
{
	LoadAutPiDll();
	InitSettings();
	RegisterPreModeElement(InitPauseScreen);
	
	ModLoader_WriteJump((void*)0x40DD70, (void*)Call_Pause);
	// ModLoader_WriteJump((void*)0x412320, (void*)ExampleFunction);
}
