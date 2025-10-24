print("Lua: loading configuration...")

config = {
    screen_width = 800,
    screen_height = 600,
    title = "SDL Lua Demo (from Lua)"
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
        b = 255,
        a = 255
    }
}

print("Lua: configuration loaded!")
