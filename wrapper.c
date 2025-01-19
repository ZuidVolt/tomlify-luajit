// wrapper.c
#include <lauxlib.h>
#include <luajit.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h> // for EXIT_FAILURE

// Include the bytecode
#include "tomlify_bc.h"

static void print_error(const char* msg) {
    if(fprintf(stderr, "Error: %s\n", msg) < 0) {
        // If we can't even print the error, abort
        abort();
    }
}

int main(int argc, char* argv[]) {
    lua_State* lua_state = luaL_newstate();
    if(!lua_state) {
        print_error("Failed to create Lua state");
        return EXIT_FAILURE;
    }

    luaL_openlibs(lua_state);

    // Pre-allocate table with known size
    lua_createtable(lua_state, argc, 0);
    for(int i = 0; i < argc; i++) {
        lua_pushstring(lua_state, argv[i]);
        lua_rawseti(lua_state, -2, i);
    }
    lua_setglobal(lua_state, "arg");

    // Load and run the embedded bytecode
    if(luaL_loadbuffer(lua_state, (const char*) luaJIT_BC_tomlify, sizeof(luaJIT_BC_tomlify), "=tomlify") ||
       lua_pcall(lua_state, 0, 0, 0)) {
        const char* error_msg = lua_tostring(lua_state, -1);
        print_error(error_msg ? error_msg : "Unknown error");
        lua_close(lua_state);
        return EXIT_FAILURE;
    }

    lua_close(lua_state);
    return EXIT_SUCCESS;
}
