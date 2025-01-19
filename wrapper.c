// wrapper.c
#include <lauxlib.h>
#include <luajit.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

#include "tomlify_bc.h"

static void print_error(const char* const MSG) {
    if(!MSG) {
        const char* const DEFAULT_MSG = "Unknown error (null message)";
        if(fprintf(stderr, "Error: %s\n", DEFAULT_MSG) < 0) {
            abort();
        }
        return;
    }
    if(fprintf(stderr, "Error: %s\n", MSG) < 0) {
        abort();
    }
}

static void check_stack(lua_State* const LUA, const int NEEDED) {
    if(!LUA) {
        return;
    }
    if(lua_checkstack(LUA, NEEDED) == 0) {
        print_error("Lua stack overflow");
        lua_close(LUA);
        exit(EXIT_FAILURE);
    }
}

static int init_lua_state(lua_State** const STATE) {
    *STATE = luaL_newstate();
    if(!*STATE) {
        print_error("Failed to create Lua state");
        return 0;
    }
    return 1;
}

static void setup_arg_table(lua_State* const LUA, const int ARGC, char* const ARGV[]) {
    check_stack(LUA, ARGC + 2);
    lua_createtable(LUA, ARGC, 0);

    for(int i = 0; i < ARGC; i++) {
        if(!ARGV[i]) {
            print_error("Null argument encountered");
            lua_close(LUA);
            exit(EXIT_FAILURE);
        }
        lua_pushstring(LUA, ARGV[i]);
        lua_rawseti(LUA, -2, i);
    }

    lua_setglobal(LUA, "arg");
}

static int run_bytecode(lua_State* LUA) {
    if(sizeof(luaJIT_BC_tomlify) == 0) {
        print_error("Invalid bytecode data");
        return 0;
    }

    const int LOAD_RESULT =
        luaL_loadbuffer(LUA, (const char*) luaJIT_BC_tomlify, sizeof(luaJIT_BC_tomlify), "=tomlify");

    if(LOAD_RESULT || lua_pcall(LUA, 0, 0, 0)) {
        const char* const ERROR_MSG = lua_tostring(LUA, -1);
        print_error(ERROR_MSG ? ERROR_MSG : "Unknown error in Lua execution");
        return 0;
    }
    return 1;
}

int main(int argc, char* argv[]) {
    if(argc < 0 || !argv) {
        print_error("Invalid arguments");
        return EXIT_FAILURE;
    }

    lua_State* lua_state;
    if(!init_lua_state(&lua_state)) {
        return EXIT_FAILURE;
    }

    luaL_openlibs(lua_state);
    setup_arg_table(lua_state, argc, argv);

    if(!run_bytecode(lua_state)) {
        lua_close(lua_state);
        return EXIT_FAILURE;
    }

    lua_close(lua_state);
    return EXIT_SUCCESS;
}
