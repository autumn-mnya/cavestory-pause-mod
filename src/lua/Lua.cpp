#include <Windows.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

extern "C"
{
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#include "Lua.h"


#include "../Main.h"
#include "../mod_loader.h"
#include "../cave_story.h"

#include "../PauseScreen.h"

#include "../AutPI.h"

#define gL GetLuaL()