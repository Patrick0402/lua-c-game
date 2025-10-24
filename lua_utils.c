#include "lua_utils.h"
#include "logger.h"
#include <lauxlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/// Split a dot path ("player.inventory[2].name") and traverse Lua tables.
static bool push_lua_field_path(lua_State *L, const char *path, bool strict)
{
    const char *start = path;
    const char *dot;

    // Puxar a tabela global inicial
    char segment[64];
    const char *first_dot = strchr(start, '.');
    size_t len = first_dot ? (size_t)(first_dot - start) : strlen(start);
    if (len >= sizeof(segment))
        len = sizeof(segment) - 1;
    memcpy(segment, start, len);
    segment[len] = '\0';

    lua_getglobal(L, segment);
    if (lua_isnil(L, -1))
    {
        if (strict)
            log_error("Lua: global '%s' not found", segment);
        return false;
    }

    start = first_dot ? first_dot + 1 : NULL;

    while (start && *start)
    {
        dot = strchr(start, '.');
        len = dot ? (size_t)(dot - start) : strlen(start);
        if (len >= sizeof(segment))
            len = sizeof(segment) - 1;
        memcpy(segment, start, len);
        segment[len] = '\0';

        lua_getfield(L, -1, segment);
        lua_remove(L, -2); // remove parent

        if (lua_isnil(L, -1))
        {
            if (strict)
                log_error("Lua: '%s' not found", segment);
            return false;
        }

        start = dot ? dot + 1 : NULL;
    }

    return true;
}

static int get_int_path(lua_State *L, const char *path, int def, bool strict)
{
    if (!push_lua_field_path(L, path, strict))
        return def;
    int val = def;
    if (lua_isinteger(L, -1))
        val = (int)lua_tointeger(L, -1);
    else if (strict)
        log_error("Lua: '%s' missing or invalid integer", path);
    else
        log_debug("Lua: '%s' missing or invalid, using default %d", path, def);
    lua_pop(L, 1);
    return val;
}

static float get_float_path(lua_State *L, const char *path, float def, bool strict)
{
    if (!push_lua_field_path(L, path, strict))
        return def;
    float val = def;
    if (lua_isnumber(L, -1))
        val = (float)lua_tonumber(L, -1);
    else if (strict)
        log_error("Lua: '%s' missing or invalid float", path);
    else
        log_debug("Lua: '%s' missing or invalid, using default %f", path, def);
    lua_pop(L, 1);
    return val;
}

static Uint8 get_component_path(lua_State *L, const char *path, Uint8 def, bool strict)
{
    if (!push_lua_field_path(L, path, strict))
        return def;
    Uint8 val = def;
    if (lua_isnumber(L, -1))
        val = (Uint8)lua_tointeger(L, -1);
    else if (strict)
        log_error("Lua: color '%s' missing or invalid", path);
    else
        log_debug("Lua: color '%s' missing or invalid, using default %d", path, def);
    lua_pop(L, 1);
    return val;
}

int get_lua_int_field(lua_State *L, const char *path, int def)
{
    return get_int_path(L, path, def, false);
}

float get_lua_float_field(lua_State *L, const char *path, float def)
{
    return get_float_path(L, path, def, false);
}

void get_lua_color_field(lua_State *L, const char *path, SDL_Color *color)
{
    if (!push_lua_field_path(L, path, false))
        return;
    if (lua_istable(L, -1))
    {
        lua_getfield(L, -1, "r");
        color->r = lua_isnumber(L, -1) ? (Uint8)lua_tointeger(L, -1) : color->r;
        lua_pop(L, 1);

        lua_getfield(L, -1, "g");
        color->g = lua_isnumber(L, -1) ? (Uint8)lua_tointeger(L, -1) : color->g;
        lua_pop(L, 1);

        lua_getfield(L, -1, "b");
        color->b = lua_isnumber(L, -1) ? (Uint8)lua_tointeger(L, -1) : color->b;
        lua_pop(L, 1);

        lua_getfield(L, -1, "a");
        color->a = lua_isnumber(L, -1) ? (Uint8)lua_tointeger(L, -1) : color->a;
        lua_pop(L, 1);
    }
    else
        log_warn("Lua: '%s' not found or not a table; using defaults.", path);
    lua_pop(L, 1);
}

void get_lua_string_field(lua_State *L, const char *path, const char *def, char *out, size_t n)
{
    if (!push_lua_field_path(L, path, false))
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
