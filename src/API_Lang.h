#pragma once

#include <Windows.h>
#include "cave_story.h"
#include <vector>

extern HMODULE langDLL;

void LoadLangDLL();
std::wstring GetLangString(std::string& string_id, const char* defaultString);
const char* GetLangString_Char(const char* string_id, const char* defaultString);
bool StringIdExists(const char* name);