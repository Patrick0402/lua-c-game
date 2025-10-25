// game.c

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
#include "logger.h"
#include "lua_utils.h"
#include <SDL2/SDL_ttf.h>

#define CONFIG_FILE "config.lua"
#define WINDOW_TITLE_MAX 128

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
lua_State *L = NULL;
bool gGameIsRunning = true;
static int gScreenWidth = 800;
static int gScreenHeight = 600;
static char gWindowTitle[WINDOW_TITLE_MAX] = "SDL Lua Space Invaders";

// ---------------- SDL Init ----------------

bool init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        LOG_ERROR("SDL could not initialize! SDL_Error: %s", SDL_GetError());
        return false;
    }
    return true;
}

bool create_window_and_renderer()
{
    gWindow = SDL_CreateWindow(gWindowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               gScreenWidth, gScreenHeight, SDL_WINDOW_SHOWN);
    if (!gWindow)
    {
        LOG_ERROR("SDL_CreateWindow failed: %s", SDL_GetError());
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!gRenderer)
    {
        LOG_ERROR("SDL_CreateRenderer failed: %s", SDL_GetError());
        return false;
    }

    if (TTF_Init() == -1)
    {
        LOG_ERROR("SDL_ttf could not initialize! %s", TTF_GetError());
        return false;
    }

    return true;
}

// ---------------- Forward Declarations ----------------
int lua_draw_rect(lua_State *L);
int lua_draw_text_center(lua_State *L);
int lua_draw_text_raw(lua_State *L);

int lua_clear_screen(lua_State *L);
int lua_present_renderer(lua_State *L);

// ---------------- Lua Init ----------------

bool init_lua()
{
    L = luaL_newstate();
    if (!L)
    {
        LOG_ERROR("Failed to create Lua state");
        return false;
    }
    luaL_openlibs(L);
    lua_register(L, "DrawRect", lua_draw_rect);
    lua_register(L, "DrawTextCenter", lua_draw_text_center);
    lua_register(L, "DrawText", lua_draw_text_raw);
    lua_register(L, "ClearScreen", lua_clear_screen);
    lua_register(L, "PresentRenderer", lua_present_renderer);

    // Carregar todos os scripts Lua necessários
    const char *scripts[] = {
        "scripts/config.lua",
        "scripts/player.lua",
        "scripts/projectile.lua",
        "scripts/enemy.lua",
        "scripts/main.lua"};

    for (int i = 0; i < sizeof(scripts) / sizeof(scripts[0]); i++)
    {
        if (luaL_dofile(L, scripts[i]) != LUA_OK)
        {
            LOG_ERROR("Error loading Lua script %s: %s", scripts[i], lua_tostring(L, -1));
            lua_pop(L, 1);
            return false;
        }
    }

    return true;
}

// ---------------- Config ----------------

void load_config_from_lua()
{
    gScreenWidth = get_lua_int_field(L, "Config.screen_width", gScreenWidth);
    gScreenHeight = get_lua_int_field(L, "Config.screen_height", gScreenHeight);
    get_lua_string_field(L, "Config.title", "SDL Lua Space Invaders", gWindowTitle, sizeof(gWindowTitle));
}

// ---------------- Initialize & Cleanup ----------------

bool initialize()
{
    if (!init_sdl())
        return false;
    if (!init_lua())
        return false;
    load_config_from_lua();
    if (!create_window_and_renderer())
        return false;
    return true;
}

void cleanup()
{
    if (L)
    {
        lua_close(L);
        L = NULL;
    }
    if (gRenderer)
    {
        SDL_DestroyRenderer(gRenderer);
        gRenderer = NULL;
    }
    if (gWindow)
    {
        SDL_DestroyWindow(gWindow);
        gWindow = NULL;
    }
    TTF_Quit();
    SDL_Quit();
}

// ---------------- Input ----------------

void handle_input()
{
    static bool prevShoot = false;
    static bool prevUp = false;
    static bool prevDown = false;

    SDL_Event e;
    bool left = false, right = false, up = false, down = false, shoot = false;

    const Uint8 *keyStates = SDL_GetKeyboardState(NULL);
    left = keyStates[SDL_SCANCODE_LEFT] || keyStates[SDL_SCANCODE_A];
    right = keyStates[SDL_SCANCODE_RIGHT] || keyStates[SDL_SCANCODE_D];
    up = keyStates[SDL_SCANCODE_UP] || keyStates[SDL_SCANCODE_W];
    down = keyStates[SDL_SCANCODE_DOWN] || keyStates[SDL_SCANCODE_S];
    shoot = keyStates[SDL_SCANCODE_SPACE];

    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            gGameIsRunning = false;
    }

    // Detecta bordas (transição de solto -> pressionado)
    bool shootPressed = (!prevShoot && shoot);
    bool upPressed = (!prevUp && up);
    bool downPressed = (!prevDown && down);

    // Atualiza anteriores
    prevShoot = shoot;
    prevUp = up;
    prevDown = down;

    // Cria tabela Input no Lua
    lua_newtable(L);
    lua_pushboolean(L, left);
    lua_setfield(L, -2, "left");
    lua_pushboolean(L, right);
    lua_setfield(L, -2, "right");
    lua_pushboolean(L, up);
    lua_setfield(L, -2, "up");
    lua_pushboolean(L, down);
    lua_setfield(L, -2, "down");
    lua_pushboolean(L, shoot);
    lua_setfield(L, -2, "shoot");

    // Novos campos — apenas quando a tecla for pressionada naquele frame
    lua_pushboolean(L, shootPressed);
    lua_setfield(L, -2, "shoot_pressed");
    lua_pushboolean(L, upPressed);
    lua_setfield(L, -2, "up_pressed");
    lua_pushboolean(L, downPressed);
    lua_setfield(L, -2, "down_pressed");

    lua_setglobal(L, "Input");
}

// ---------------- Update ----------------

void update(float dt)
{
    lua_getglobal(L, "UpdateGame");
    lua_pushnumber(L, dt);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK)
    {
        LOG_ERROR("Lua update_game error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

// ---------------- Render ----------------

void render()
{
    lua_getglobal(L, "Render");
    lua_pushlightuserdata(L, gRenderer);
    if (lua_pcall(L, 1, 0, 0) != LUA_OK)
    {
        LOG_ERROR("Lua render error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
    // SDL_RenderPresent já é chamado dentro de Lua
}

int lua_draw_rect(lua_State *L)
{
    SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
    int x = luaL_checkinteger(L, 2);
    int y = luaL_checkinteger(L, 3);
    int w = luaL_checkinteger(L, 4);
    int h = luaL_checkinteger(L, 5);
    int r = luaL_checkinteger(L, 6);
    int g = luaL_checkinteger(L, 7);
    int b = luaL_checkinteger(L, 8);
    int a = luaL_checkinteger(L, 9);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
    return 0;
}

int lua_draw_text_raw(lua_State *L)
{
    SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
    const char *text = luaL_checkstring(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int g = luaL_checkinteger(L, 6);
    int b = luaL_checkinteger(L, 7);
    int a = luaL_checkinteger(L, 8);

    static TTF_Font *font = NULL;
    if (!font)
    {
        font = TTF_OpenFont("assets/Roboto-Bold.ttf", 24);
        if (!font)
        {
            LOG_ERROR("Failed to load font: %s", TTF_GetError());
            return 0;
        }
    }

    SDL_Color color = {r, g, b, a};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface) return 0;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return 0;
    }

    SDL_Rect dest = {x, y, surface->w, surface->h};
    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    return 0;
}

int lua_draw_text_center(lua_State *L)
{
    SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
    const char *text = luaL_checkstring(L, 2);
    int x = luaL_checkinteger(L, 3);
    int y = luaL_checkinteger(L, 4);
    int r = luaL_checkinteger(L, 5);
    int g = luaL_checkinteger(L, 6);
    int b = luaL_checkinteger(L, 7);
    int a = luaL_checkinteger(L, 8);

    static TTF_Font *font = NULL;
    if (!font)
    {
        font = TTF_OpenFont("assets/Roboto-Bold.ttf", 32);
        if (!font)
        {
            LOG_ERROR("Failed to load font: %s", TTF_GetError());
            return 0;
        }
    }

    SDL_Color color = {r, g, b, a};
    SDL_Surface *surface = TTF_RenderText_Blended(font, text, color);
    if (!surface)
        return 0;

    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture)
    {
        SDL_FreeSurface(surface);
        return 0;
    }

    SDL_Rect dest;
    dest.w = surface->w;
    dest.h = surface->h;
    dest.x = x - surface->w / 2; // centraliza horizontalmente
    dest.y = y - surface->h / 2; // centraliza verticalmente

    SDL_RenderCopy(renderer, texture, NULL, &dest);

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);

    return 0;
}

int lua_clear_screen(lua_State *L)
{
    SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
    int r = luaL_checkinteger(L, 2);
    int g = luaL_checkinteger(L, 3);
    int b = luaL_checkinteger(L, 4);
    int a = luaL_checkinteger(L, 5);

    SDL_SetRenderDrawColor(renderer, r, g, b, a);
    SDL_RenderClear(renderer);
    return 0;
}

int lua_present_renderer(lua_State *L)
{
    SDL_Renderer *renderer = (SDL_Renderer *)lua_touserdata(L, 1);
    SDL_RenderPresent(renderer);
    return 0;
}

// ---------------- Main Loop ----------------

int main(int argc, char **argv)
{
    log_init("game.log", LOG_DEBUG);
    LOG_INFO("Game starting...");

    if (!initialize())
    {
        LOG_ERROR("Initialization failed!");
        cleanup();
        return 1;
    }

    int lastTime = SDL_GetTicks();

    while (gGameIsRunning)
    {
        int currentTime = SDL_GetTicks();
        float dt = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handle_input();
        update(dt);

        // Verifica se Lua quer sair
        lua_getglobal(L, "GameState");
        lua_getfield(L, -1, "current");
        const char *state = lua_tostring(L, -1);
        if (state && strcmp(state, "quit") == 0)
        {
            gGameIsRunning = false;
        }
        lua_pop(L, 2); // limpa pilha (GameState e current)

        render();
    }
    LOG_INFO("Game shutting down...");
    cleanup();
    log_shutdown();
    return 0;
}
