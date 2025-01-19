// wrapper.c
#include <lauxlib.h>
#include <luajit.h>
#include <lualib.h>
#include <stdio.h>
#include <stdlib.h>

#include "tomlify_bc.h"

static void print_error(const char* msg) {
    if(!msg) {
        msg = "Unknown error (null message)";
    }
    if(fprintf(stderr, "Error: %s\n", msg) < 0) {
        abort();
    }
}

static void check_stack(lua_State* lua, int needed) {
    if(!lua) {
        return;
    }
    if(lua_checkstack(lua, needed) == 0) {
        print_error("Lua stack overflow");
        lua_close(lua);
        exit(EXIT_FAILURE);
    }
}

static int init_lua_state(lua_State** state) {
    *state = luaL_newstate();
    if(!*state) {
        print_error("Failed to create Lua state");
        return 0;
    }
    return 1;
}

static void setup_arg_table(lua_State* lua, int argc, char* argv[]) {
    check_stack(lua, argc + 2);
    lua_createtable(lua, argc, 0);

    for(int i = 0; i < argc; i++) {
        if(!argv[i]) {
            print_error("Null argument encountered");
            lua_close(lua);
            exit(EXIT_FAILURE);
        }
        lua_pushstring(lua, argv[i]);
        lua_rawseti(lua, -2, i);
    }

    lua_setglobal(lua, "arg");
}

static int run_bytecode(lua_State* lua) {
    if(sizeof(luaJIT_BC_tomlify) == 0) {
        print_error("Invalid bytecode data");
        return 0;
    }

    int load_result = luaL_loadbuffer(lua, (const char*) luaJIT_BC_tomlify, sizeof(luaJIT_BC_tomlify), "=tomlify");

    if(load_result || lua_pcall(lua, 0, 0, 0)) {
        const char* error_msg = lua_tostring(lua, -1);
        print_error(error_msg ? error_msg : "Unknown error in Lua execution");
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
