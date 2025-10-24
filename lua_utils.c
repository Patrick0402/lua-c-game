#include "lua_utils.h"
#include "logger.h"
#include <lauxlib.h>
#include <string.h>
#include <stdio.h>

/// Split a dot path ("player.color.r") and traverse Lua tables.
static bool push_lua_field_path(lua_State *L, const char *path)
{
    const char *start = path;
    const char *dot;
    while ((dot = strchr(start, '.')) != NULL)
    {
        char key[64];
        size_t len = dot - start;
        if (len >= sizeof(key))
            len = sizeof(key) - 1;
        memcpy(key, start, len);
        key[len] = '\0';

        lua_getfield(L, -1, key);
        if (!lua_istable(L, -1))
        {
            log_warn("Lua: '%s' not found or not a table.", key);
            lua_pop(L, 1);
            return false;
        }
        lua_remove(L, -2); // pop parent, keep child
        start = dot + 1;
    }

    lua_getfield(L, -1, start);
    return true;
}

static int get_int_path(lua_State *L, const char *path, int def)
{
    if (!push_lua_field_path(L, path))
        return def;
    int val = def;
    if (lua_isinteger(L, -1))
        val = (int)lua_tointeger(L, -1);
    else
        log_debug("Lua: '%s' missing or invalid, using default %d", path, def);
    lua_pop(L, 1);
    return val;
}

static float get_float_path(lua_State *L, const char *path, float def)
{
    if (!push_lua_field_path(L, path))
        return def;
    float val = def;
    if (lua_isnumber(L, -1))
        val = (float)lua_tonumber(L, -1);
    else
        log_debug("Lua: '%s' missing or invalid, using default %f", path, def);
    lua_pop(L, 1);
    return val;
}

static Uint8 get_component_path(lua_State *L, const char *path, Uint8 def)
{
    if (!push_lua_field_path(L, path))
        return def;
    Uint8 val = def;
    if (lua_isnumber(L, -1))
        val = (Uint8)lua_tointeger(L, -1);
    else
        log_debug("Lua: color '%s' missing or invalid, using default %d", path, def);
    lua_pop(L, 1);
    return val;
}

int get_lua_int_field(lua_State *L, const char *path, int def)
{
    return get_int_path(L, path, def);
}

float get_lua_float_field(lua_State *L, const char *path, float def)
{
    return get_float_path(L, path, def);
}

void get_lua_color_field(lua_State *L, const char *path, SDL_Color *color)
{
    if (!push_lua_field_path(L, path))
        return;
    if (lua_istable(L, -1))
    {
        color->r = get_component_path(L, "r", color->r);
        color->g = get_component_path(L, "g", color->g);
        color->b = get_component_path(L, "b", color->b);
        color->a = get_component_path(L, "a", color->a);
    }
    else
        log_warn("Lua: '%s' not found or not a table; using defaults.", path);
    lua_pop(L, 1);
}

void get_lua_string_field(lua_State *L, const char *path, const char *def, char *out, size_t n)
{
    if (!push_lua_field_path(L, path))
    {
        snprintf(out, n, "%s", def);
        return;
    }
    const char *val = lua_isstring(L, -1) ? lua_tostring(L, -1) : def;
    snprintf(out, n, "%s", val);
    lua_pop(L, 1);
}

bool reload_lua_config(lua_State *L, const char *file)
{
    log_info("Reloading Lua configuration from %s", file);
    lua_settop(L, 0);
    if (luaL_dofile(L, file) != LUA_OK)
    {
        log_error("Lua reload failed: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
        return false;
    }
    lua_gc(L, LUA_GCCOLLECT, 0);
    return true;
}
