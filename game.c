#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

#include <lua.h>     //Core Lua functions
#include <lualib.h>  //Standard Lua libraries (print, math, etc.)
#include <lauxlib.h> //Helper function for lua APIs

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define CONFIG_FILE "config.lua"

typedef struct
{
    float x, y; // Position and size (x, y, w, h)
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

int get_lua_int_field(lua_State *L, const char *key, int default_value)
{
    int result = default_value;
    lua_pushstring(L, key);

    lua_gettable(L, -2);

    if (lua_isnumber(L, -1))
    {
        result = (int)lua_tonumber(L, -1);
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
    lua_pushstring(L, key);

    lua_gettable(L, -2);

    if (lua_isnumber(L, -1))
    {
        result = (float)lua_tonumber(L, -1);
    }
    else
    {
        fprintf(stderr, "Warning: Lua config '%s' not found or not a number. Using default %d.\n", key, default_value);
    }

    lua_pop(L, 1);
    return result;
}

void get_lua_color_field(lua_State *l, const char *key, SDL_Color *color)
{
    lua_pushstring(l, key);
    lua_gettable(l, -2);
    if (lua_istable(l, -1))
    {
        lua_getfield(l, -1, "r");
        if (lua_isnumber(l, -1))
            color->r = (Uint8)lua_tonumber(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'r' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "g");
        if (lua_isnumber(l, -1))
            color->g = (Uint8)lua_tonumber(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'g' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "b");
        if (lua_isnumber(l, -1))
            color->b = (Uint8)lua_tonumber(l, -1);
        else
            fprintf(stderr, "Warning: Lua config: color field 'b' missing/invalid.\n");
        lua_pop(l, 1);

        lua_getfield(l, -1, "a");
        if (lua_isnumber(l, -1))
            color->a = (Uint8)lua_tonumber(l, -1);
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

bool initialize()
{
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create window
    gWindow = SDL_CreateWindow("SDL Lua Demo", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Create renderer
    gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (gRenderer == NULL)
    {
        fprintf(stderr, "Renderer could be created! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // Initialize Lua
    L = luaL_newstate();
    if (L == NULL)
    {
        fprintf(stderr, "Failed to create Lua state\n");
        return false;
    }
    luaL_openlibs(L);

    printf("C: Loading Lua configuration from %s...\n", CONFIG_FILE);
    if (luaL_dofile(L, CONFIG_FILE) != LUA_OK)
    {
        fprintf(stderr, "Error loading Lua script %s: %s\n", CONFIG_FILE, lua_tostring(L, -1));
        // Continue with defaults, only log the errors...
    }
    else
    {
        printf("C: Lua script executed successfully.\n");

        lua_getglobal(L, "player");

        if (lua_istable(L, -1))
        {
            printf("C: Found 'player' table in Lua\n");

            gPlayer.x = get_lua_float_field(L, "x", 0);
            gPlayer.y = get_lua_float_field(L, "y", 0);
            gPlayer.w = get_lua_int_field(L, "width", 50);
            gPlayer.h = get_lua_int_field(L, "height", 50);
            gPlayer.speed = get_lua_float_field(L, "speed", 100.0f);

            gPlayer.color = (SDL_Color){255, 0, 0, 255}; // Default

            get_lua_color_field(L, "color", &gPlayer.color);

            printf("C: Loaded player config: pos=(%d,%d), size=(%d,%d), speed=%.1f, color=(%d,%d,%d,%d)\n",
                   gPlayer.x, gPlayer.y, gPlayer.w, gPlayer.h,
                   gPlayer.speed, gPlayer.color.r, gPlayer.color.g, gPlayer.color.b, gPlayer.color.a);
        } else{
            fprintf(stderr, "Warning: Lua global 'player' not found or not a table. Using default values.\n");
            gPlayer.x = 0;
            gPlayer.y = 0;
            gPlayer.w = 50;
            gPlayer.h = 50;
            gPlayer.speed = 100.0f;
            gPlayer.color = (SDL_Color){255, 0, 0, 255};
        }
        lua_pop(L, -1);
    }

    // Initialize other player states.
    gPlayer.dx = 0.0f;
    gPlayer.dy = 0.0f;

    return true;
}

void cleanup()
{
    printf("C: Cleaning up...");

    // Close Lua state
    if(L){
        lua_close(L);
        L = NULL;
        printf("C: Lua state closed.\n");
    }

    // Destroy SDL resources
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

    // Continuous key state checking for smooth movement
    const Uint8 *currentKeyStates = SDL_GetKeyboardState(NULL);
    gPlayer.dx = 0;
    gPlayer.dy = 0;

    if (currentKeyStates[SDL_SCANCODE_UP] || currentKeyStates[SDL_SCANCODE_W])
    {
        gPlayer.dy = -1.0f;
    }
    if (currentKeyStates[SDL_SCANCODE_DOWN] || currentKeyStates[SDL_SCANCODE_S])
    {
        gPlayer.dy = 1.0f;
    }
    if (currentKeyStates[SDL_SCANCODE_LEFT] || currentKeyStates[SDL_SCANCODE_A])
    {
        gPlayer.dx = -1.0f;
    }
    if (currentKeyStates[SDL_SCANCODE_RIGHT] || currentKeyStates[SDL_SCANCODE_D])
    {
        gPlayer.dx = 1.0f;
    }
}

void update(float deltaTime)
{
    // Diagonal normalization
    float length = sqrtf(gPlayer.dx * gPlayer.dx + gPlayer.dy * gPlayer.dy);
    if (length > 0.0f)
    {
        gPlayer.dx /= length;
        gPlayer.dy /= length;
    }

    // Update player position based on velocity speed
    // deltaTime helps make movemnt frame-rate independent
    gPlayer.x += (gPlayer.dx * gPlayer.speed * deltaTime);
    gPlayer.y += (gPlayer.dy * gPlayer.speed * deltaTime);

    // Basic boundary colision
    if (gPlayer.x < 0)
        gPlayer.x = 0;
    if (gPlayer.y < 0)
        gPlayer.y = 0;
    if (gPlayer.x + gPlayer.w > SCREEN_WIDTH)
        gPlayer.x = SCREEN_WIDTH - gPlayer.w;
    if (gPlayer.y + gPlayer.h > SCREEN_HEIGHT)
        gPlayer.y = SCREEN_HEIGHT - gPlayer.h;
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
        printf("Failed to initialize!\n");
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