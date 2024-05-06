#include <windows.h>
#include <iostream>
#include "AutPI.h"
#include "mod_loader.h"
#include "cave_story.h"
#include <string>
#include "API_Lang.h"

HMODULE langDLL;

void LoadLangDLL()
{
    langDLL = LoadLibrary("lang.dll");
    if (langDLL == nullptr)
        printf("%s\n", "Failed to load lang.dll");
    else
        printf("%s\n", "Loaded lang.dll!");
}

typedef std::string(*GetStringValueFunc)(const std::string&);

std::string get_string_value_auto(const std::string& string_id, const char* defaultString)
{
    std::string result;

    if (langDLL != nullptr) {
        // Get the function pointer
        GetStringValueFunc getStringValueAuto = reinterpret_cast<GetStringValueFunc>(GetProcAddress(langDLL, "get_string_value_auto"));
        if (getStringValueAuto != nullptr) {
            // Call the function
            result = getStringValueAuto(string_id);
        }
        else {
            std::cerr << "Failed to get function pointer." << std::endl;
            result = std::string(defaultString, defaultString + strlen(defaultString));
        }
    }
    else {
        // std::cerr << "DLL handle is nullptr." << std::endl;
        result = std::string(defaultString, defaultString + strlen(defaultString));
    }

    return result;
}

typedef const char* (*GetStringCharFunc)(const char*);

const char* GetLangString_Char(const char* string_id, const char* defaultString)
{
    const char* result = defaultString;

    if (langDLL != nullptr) {
        // Get the function pointer
        GetStringCharFunc getStringValueAuto = reinterpret_cast<GetStringCharFunc>(GetProcAddress(langDLL, "GetStringChar"));
        if (getStringValueAuto != nullptr) {
            // Call the function
            result = getStringValueAuto(string_id);
        }
        else {
            std::cerr << "Failed to get function pointer. Error code: " << GetLastError() << std::endl;
        }
    }
    else {
        // std::cerr << "DLL handle is nullptr." << std::endl;
    }

    return result;
}

typedef bool (*StringIdExistsFunc)(const char*);

bool StringIdExists(const char* name)
{
    bool result = false;
    if (langDLL != nullptr) {
        StringIdExistsFunc func = reinterpret_cast<StringIdExistsFunc>(GetProcAddress(langDLL, "StringIdExists"));
        if (func != nullptr) {
            result = func(name);
        }
        else {
            std::cerr << "Failed to get function pointer. Error code: " << GetLastError() << std::endl;
        }
    }
    else {

    }
    return result;
}