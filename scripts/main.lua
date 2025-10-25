-- main.lua

-- Funções nativas mockadas (para evitar alertas no editor)
Input = Input or {}
DrawRect = DrawRect or function(...) end
DrawText = DrawText or function(...) end
ClearScreen = ClearScreen or function(...) end
PresentRenderer = PresentRenderer or function(...) end

-- Carregar módulos
require("scripts.game_state") -- define Config, GameState, e spawner
require("scripts.menu")
require("scripts.player")
require("scripts.projectile")
require("scripts.enemy")


GameState = {
    current = "menu"
}
Score = 0
GameOverOptions = { "Jogar novamente", "Menu principal" }
GameOverSelected = 1
local gameover_shoot_was_pressed = false


math.randomseed(os.time())


-- Inicialização
function ResetGame()
    Projectiles = {}
    EnemyProjectiles = {}
    Enemies = {}
    Player:reset()
    ResetEnemySystem()
    ResetDifficulty()
    Score = 0;
end

function UpdateGame(dt)
    local state = GameState.current

    if state == "menu" then
        UpdateMenu(Input)
        return
    elseif state == "playing" then
        Player:update(dt, Input)
        UpdateProjectiles(dt)
        UpdateEnemies(dt)
        UpdateEnemyProjectiles(dt)
        HandleSpawning(dt)
        HandleDifficulty(dt)

        if Player.hp <= 0 then
            GameState.current = "gameover"
        end
    elseif state == "gameover" then
        UpdateGameOverMenu(Input)
    end
end

function Render(renderer)
    ClearScreen(renderer, 0, 0, 0, 255)

    local state = GameState.current
    if state == "menu" then
        DrawMenu(renderer)
    elseif state == "playing" then
        DrawPlayer(renderer)
        DrawEnemies(renderer)
        DrawProjectiles(renderer)
        DrawEnemyProjectiles(renderer)

        DrawText(renderer, "Score: " .. Score, 10, 10, 255, 255, 255, 255)
        DrawText(renderer, "HP: " .. Player.hp, 10, 40, 255, 255, 255, 255)
    elseif state == "gameover" then
        DrawGameOverMenu(renderer)
    end

    PresentRenderer(renderer)
end

