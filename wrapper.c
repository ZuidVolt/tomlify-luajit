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
    lua_State* L = luaL_newstate();
    if(!L) {
        print_error("Failed to create Lua state");
        return EXIT_FAILURE;
    }

    luaL_openlibs(L);

    // Pre-allocate table with known size
    lua_createtable(L, argc, 0);
    for(int i = 0; i < argc; i++) {
        lua_pushstring(L, argv[i]);
        lua_rawseti(L, -2, i);
    }
    lua_setglobal(L, "arg");

    // Load and run the embedded bytecode
    if(luaL_loadbuffer(L, (const char*) luaJIT_BC_tomlify, sizeof(luaJIT_BC_tomlify), "=tomlify") ||
       lua_pcall(L, 0, 0, 0)) {
        const char* error_msg = lua_tostring(L, -1);
        print_error(error_msg ? error_msg : "Unknown error");
        lua_close(L);
        return EXIT_FAILURE;
    }

    lua_close(L);
    return EXIT_SUCCESS;
}
