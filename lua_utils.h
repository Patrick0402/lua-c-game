#ifndef LUA_UTILS_H
#define LUA_UTILS_H

#include <SDL2/SDL.h>
#include <lua.h>
#include <stdbool.h>

// Function declarations
int get_lua_int_field(lua_State *L, const char *key, int default_value);
float get_lua_float_field(lua_State *L, const char *key, float default_value);
void get_lua_color_field(lua_State *L, const char *key, SDL_Color *color);
void get_lua_string_field(lua_State *L, const char *key, const char *default_value, char *out_buf, size_t buf_size);
bool reload_lua_config(lua_State *L, const char *filename);

#endif // LUA_UTILS_H
