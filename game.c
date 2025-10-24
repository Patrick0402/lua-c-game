#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <SDL2/SDL.h>

#include <lua.h>     //Core Lua functions
#include <lualib.h>  //Standard Lua libraries (print, math, etc.)
#include <lauxlib.h> //Helper function for lua APIs

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CONFIG_FILE "config.lua"
#define WINDOW_TITLE_MAX 128

typedef struct
{
    float x, y; // Position
    int w, h;
    SDL_Color color; // RGBA Color
    float speed;     // Movement speed in pixels/second
    float dx;        // Current velocity x
    float dy;        // Current velocity y
} Player;

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;
lua_State *L = NULL;
Player gPlayer;
bool gGameIsRunning = true;

// Now screen size and title are configurable (defaults below, can be overridden by Lua)
static int gScreenWidth = SCREEN_WIDTH;
static int gScreenHeight = SCREEN_HEIGHT;
static char gWindowTitle[WINDOW_TITLE_MAX] = "SDL Lua Demo";

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

void Player_set_defaults(Player *p)
{
    p->x = 0.0f;
    p->y = 0.0f;
    p->w = 50;
    p->h = 50;
    p->speed = 100.0f;
    p->color.r = 255;
    p->color.g = 0;
    p->color.b = 0;
    p->color.a = 255;
    p->dx = p->dy = 0.0f;
}

bool init_sdl()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool create_window_and_renderer()
{
    gWindow = SDL_CreateWindow(gWindowTitle, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, gScreenWidth, gScreenHeight, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }
    return true;
}

bool init_lua()
{
    L = luaL_newstate();
    if (L == NULL)
    {
        fprintf(stderr, "Failed to create Lua state\n");
        return false;
    }
    luaL_openlibs(L);

    if (luaL_dofile(L, CONFIG_FILE) != LUA_OK)
    {
        fprintf(stderr, "Error loading Lua script %s: %s\n", CONFIG_FILE, lua_tostring(L, -1));
        // keep going with defaults
        lua_pop(L, 1);
    }
    return true;
}

void load_config_from_lua()
{
    // Load general config table if available
    lua_getglobal(L, "config");
    if (lua_istable(L, -1))
    {
        gScreenWidth = get_lua_int_field(L, "screen_width", gScreenWidth);
        gScreenHeight = get_lua_int_field(L, "screen_height", gScreenHeight);
        get_lua_string_field(L, "title", "SDL Lua Demo", gWindowTitle, sizeof(gWindowTitle));
    }
    lua_pop(L, 1);

    // Load player table if available
    lua_getglobal(L, "player");
    if (lua_istable(L, -1))
    {
        gPlayer.x = get_lua_float_field(L, "x", gPlayer.x);
        gPlayer.y = get_lua_float_field(L, "y", gPlayer.y);
        gPlayer.w = get_lua_int_field(L, "width", gPlayer.w);
        gPlayer.h = get_lua_int_field(L, "height", gPlayer.h);
        gPlayer.speed = get_lua_float_field(L, "speed", gPlayer.speed);

        get_lua_color_field(L, "color", &gPlayer.color);

        printf("C: Loaded player config: pos=(%.1f,%.1f), size=(%d,%d), speed=%.1f, color=(%d,%d,%d,%d)\n",
               gPlayer.x, gPlayer.y, gPlayer.w, gPlayer.h,
               gPlayer.speed, gPlayer.color.r, gPlayer.color.g, gPlayer.color.b, gPlayer.color.a);
    }
    else
    {
        fprintf(stderr, "Warning: Lua global 'player' not found or not a table. Using default values.\n");
    }
    lua_pop(L, 1); // pop player (or nil)
}

bool initialize()
{
    // SDL
    if (!init_sdl())
        return false;

    // Lua
    if (!init_lua())
    {
        // proceed but L may be NULL; we guard uses later
        return false;
    }

    // Set defaults and then override from Lua
    Player_set_defaults(&gPlayer);
    load_config_from_lua();

    // Create window and renderer with possibly overridden sizes/title
    if (!create_window_and_renderer())
        return false;

    return true;
}

void cleanup()
{
    printf("C: Cleaning up...");

    if (L)
    {
        lua_close(L);
        L = NULL;
        printf("C: Lua state closed.\n");
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

    SDL_Quit();
    printf("C: SDL Quit.\n");
}

void handleInput()
{
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
    {
        if (e.type == SDL_QUIT)
        {
            gGameIsRunning = false;
        }
    }

    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    gPlayer.dx = 0;
    gPlayer.dy = 0;

    if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W]) gPlayer.dy = -1.0f;
    if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S]) gPlayer.dy = 1.0f;
    if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A]) gPlayer.dx = -1.0f;
    if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D]) gPlayer.dx = 1.0f;
}

void update(float deltaTime)
{
    float length = sqrtf(gPlayer.dx * gPlayer.dx + gPlayer.dy * gPlayer.dy);
    if (length > 0.0f)
    {
        gPlayer.dx /= length;
        gPlayer.dy /= length;
    }

    gPlayer.x += (gPlayer.dx * gPlayer.speed * deltaTime);
    gPlayer.y += (gPlayer.dy * gPlayer.speed * deltaTime);

    // Boundaries use configurable screen size
    if (gPlayer.x < 0) gPlayer.x = 0;
    if (gPlayer.y < 0) gPlayer.y = 0;
    if (gPlayer.x + gPlayer.w > gScreenWidth) gPlayer.x = gScreenWidth - gPlayer.w;
    if (gPlayer.y + gPlayer.h > gScreenHeight) gPlayer.y = gScreenHeight - gPlayer.h;
}

void render()
{
    // 1. Clear screen (background color)
    SDL_SetRenderDrawColor(gRenderer, 0x33, 0x33, 0x33, 0xFF);
    // Dark gray
    SDL_RenderClear(gRenderer);

    // 2. Draw the player retangle (using color from Lua config)
    SDL_SetRenderDrawColor(gRenderer, gPlayer.color.r, gPlayer.color.g, gPlayer.color.b, gPlayer.color.a);
    SDL_Rect rect = {
        (int)gPlayer.x,
        (int)gPlayer.y,
        gPlayer.w,
        gPlayer.h};
    SDL_RenderFillRect(gRenderer, &rect);

    // 3. Update screen
    SDL_RenderPresent(gRenderer);
}

int main(int argc, char const *argv[])
{
    if (!initialize())
    {
        fprintf(stderr, "Failed to initialize!\n");
        cleanup();
        return 1;
    }

    int lastTime = SDL_GetTicks();

    while (gGameIsRunning)
    {
        int currentTime = SDL_GetTicks();
        float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;

        handleInput();
        update(deltaTime);
        render();
    }

    cleanup();
    return 0;
}