print("Lua: loading configuration...")

config = {
    screen_width = 800,
    screen_height = 600,
    title = "SDL Lua Demo (from Lua)",

    -- New logging configuration
    log = {
        level = "debug", -- debug, info, warn, error
        file = "game.log"
    }
}

player = {
    x = 390,
    y = 290,
    width = 20,
    height = 20,
    speed = 300.0,

    color = {
        r = 255,
        g = 0,
        b = 0,
        a = 255
    }
}

print("Lua: configuration loaded!")
