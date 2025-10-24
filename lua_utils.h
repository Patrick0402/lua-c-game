#ifndef LUA_UTILS_H
#define LUA_UTILS_H

#include <SDL2/SDL.h>
#include <lua.h>
#include <stdbool.h>
#include <stddef.h> // size_t

// Funções para ler campos Lua com suporte a caminhos (dot notation)
int get_lua_int_field(lua_State *L, const char *path, int default_value);
float get_lua_float_field(lua_State *L, const char *path, float default_value);
void get_lua_color_field(lua_State *L, const char *path, SDL_Color *color);
void get_lua_string_field(lua_State *L, const char *path, const char *default_value, char *out_buf, size_t buf_size);

// Recarregar configuração Lua de um arquivo
bool reload_lua_config(lua_State *L, const char *filename);

#endif // LUA_UTILS_H
