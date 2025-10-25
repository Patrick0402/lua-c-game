GameState = GameState or {}
GameState.current = "menu"

-- Configurações globais
Config = {
    screen_width = 800,
    screen_height = 600,
}

-- Controle de dificuldade
local spawn_timer = 0
local spawn_interval = 2
local difficulty_timer = 0
local difficulty_interval = 10
local spawn_positions = {100,200,300,400,500,600,700}
local speed_multiplier = 1

function ResetDifficulty()
    spawn_timer = 0
    spawn_interval = 2
    difficulty_timer = 0
    speed_multiplier = 1
end

function HandleSpawning(dt)
    spawn_timer = spawn_timer - dt
    if spawn_timer <= 0 then
        local x = spawn_positions[math.random(#spawn_positions)]
        local y = -30
        SpawnEnemy(x, y)  -- SpawnEnemy já vai aplicar speed_multiplier
        spawn_timer = spawn_interval
    end
end

function HandleDifficulty(dt)
    difficulty_timer = difficulty_timer + dt
    if difficulty_timer > difficulty_interval then
        speed_multiplier = speed_multiplier * 1.1
        difficulty_timer = 0
        if spawn_interval > 0.5 then
            spawn_interval = spawn_interval - 0.2
        end
    end
end

function GetSpeedMultiplier()
    return speed_multiplier
end
