#include "lua_utils.h"
#include "logger.h"
#include <lauxlib.h>
#include <string.h>
#include <stdio.h>


int get_lua_int_field(lua_State *L, const char *key, int default_value)
{
    int result = default_value;
    lua_getfield(L, -1, key); // get table[key] (expects table at top of stack)

    if (lua_isnumber(L, -1))
    {
        result = (int)lua_tointeger(L, -1);
    }
    else
    {
        fprintf(stderr, "Warning: Lua config '%s' not found or not a number. Using default %d.\n", key, default_value);
    }

    lua_pop(L, 1);
    return result;
}

float get_lua_float_field(lua_State *L, const char *key, float default_value)
{
    float result = default_value;
    lua_getfield(L, -1, key);

    if (lua_isnumber(L, -1))
    {
        result = (float)lua_tonumber(L, -1);
    }
    else
    {
        fprintf(stderr, "Warning: Lua config '%s' not found or not a number. Using default %f.\n", key, default_value);
    }

    lua_pop(L, 1);
    return result;
}

void get_lua_color_field(lua_State *l, const char *key, SDL_Color *color)
{
    lua_getfield(l, -1, key);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, "r");
        if (lua_isnumber(l, -1))
            color->r = (Uint8)lua_tointeger(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'r' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "g");
        if (lua_isnumber(l, -1))
            color->g = (Uint8)lua_tointeger(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'g' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "b");
        if (lua_isnumber(l, -1))
            color->b = (Uint8)lua_tointeger(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'b' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "a");
        if (lua_isnumber(l, -1))
            color->a = (Uint8)lua_tointeger(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'a' missing/invalid.\n");
        lua_pop(l, 1);
    }
    else
    {
        fprintf(stderr, "Warning: Lua config: '%s' field not found or not a table.\n", key);
    }
    lua_pop(l, 1);
}

void get_lua_string_field(lua_State *L, const char *key, const char *default_value, char *out_buf, size_t buf_size)
{
    lua_getfield(L, -1, key);
    if (lua_isstring(L, -1))
    {
        const char *s = lua_tostring(L, -1);
        strncpy(out_buf, s, buf_size - 1);
        out_buf[buf_size - 1] = '\0';
    }
    else
    {
        fprintf(stderr, "Warning: Lua config '%s' not found or not a string. Using default '%s'.\n", key, default_value);
        strncpy(out_buf, default_value, buf_size - 1);
        out_buf[buf_size - 1] = '\0';
    }
    lua_pop(L, 1);
}

bool reload_lua_config(lua_State *L, const char *filename) {
    log_info("Reloading Lua configuration from %s", filename);
    
    lua_settop(L, 0);  // Clear stack
    
    if (luaL_dofile(L, filename) != LUA_OK) {
        log_error("Failed to reload Lua script: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    
    return true;
}
