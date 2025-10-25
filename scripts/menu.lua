MenuOptions = { "Start", "Exit" }
MenuSelected = 1
local shoot_was_pressed = false

function UpdateMenu(input)
    if input.up_pressed then
        MenuSelected = MenuSelected - 1
        if MenuSelected < 1 then MenuSelected = #MenuOptions end
    elseif input.down_pressed then
        MenuSelected = MenuSelected + 1
        if MenuSelected > #MenuOptions then MenuSelected = 1 end
    end

    if input.shoot and not shoot_was_pressed then
        shoot_was_pressed = true
        local choice = MenuOptions[MenuSelected]
        if choice == "Start" then
            GameState.current = "playing"
            ResetGame()
        elseif choice == "Exit" then
            GameState.current = "quit"
        end
    elseif not input.shoot then
        shoot_was_pressed = false
    end
end

function DrawMenu(renderer)
    DrawRect(renderer, 0, 0, 800, 600, 10, 10, 40, 255)
    DrawRect(renderer, 150, 100, 500, 80, 30, 30, 100, 255)
    DrawRect(renderer, 160, 110, 480, 60, 0, 0, 0, 255)
    DrawText(renderer, "SPACE INVADERS LUA", 180, 120, 255, 255, 255, 255)

    local startY = 260
    local spacing = 90
    local time = os.clock()
    for i, option in ipairs(MenuOptions) do
        local isSelected = (i == MenuSelected)
        local x, y, w, h = 300, startY + (i - 1) * spacing, 200, 70

        if isSelected then
            local pulse = math.floor(math.abs(math.sin(time * 3)) * 100)
            local border = 5
            DrawBorder(renderer, x - border, y - border, w + border * 2, h + border * 2,
                border, 255, 255 - pulse, 100, 255)
            DrawRect(renderer, x, y, w, h, 60, 60, 20 + pulse, 255)
        else
            DrawRect(renderer, x, y, w, h, 0, 100, 0, 255)
        end
        DrawText(renderer, option, x + 40, y + 20, 255, 255, 255, 255)
    end
end

function DrawBorder(renderer, x, y, w, h, thickness, r, g, b, a)
    DrawRect(renderer, x, y, w, thickness, r, g, b, a)
    DrawRect(renderer, x, y + h - thickness, w, thickness, r, g, b, a)
    DrawRect(renderer, x, y, thickness, h, r, g, b, a)
    DrawRect(renderer, x + w - thickness, y, thickness, h, r, g, b, a)
end

function UpdateGameOverMenu(input)
    if input.up_pressed then
        GameOverSelected = GameOverSelected - 1
        if GameOverSelected < 1 then GameOverSelected = #GameOverOptions end
    elseif input.down_pressed then
        GameOverSelected = GameOverSelected + 1
        if GameOverSelected > #GameOverOptions then GameOverSelected = 1 end
    end

    if input.shoot and not gameover_shoot_was_pressed then
        gameover_shoot_was_pressed = true
        local choice = GameOverOptions[GameOverSelected]
        if choice == "Jogar novamente" then
            GameState.current = "playing"
            ResetGame()
        elseif choice == "Menu principal" then
            GameState.current = "menu"
        end
    elseif not input.shoot then
        gameover_shoot_was_pressed = false
    end
end

function DrawGameOverMenu(renderer)
    -- Fundo
    DrawRect(renderer, 0, 0, 800, 600, 50, 0, 0, 255)

    -- Texto da pontuação
    DrawText(renderer, "GAME OVER", 300, 150, 255, 255, 255, 255)
    DrawText(renderer, "Pontuação final: " .. Score, 300, 200, 255, 255, 0, 255)

    -- Opções
    local startY = 300
    local spacing = 90
    local time = os.clock()
    for i, option in ipairs(GameOverOptions) do
        local isSelected = (i == GameOverSelected)
        local x, y, w, h = 300, startY + (i - 1) * spacing, 200, 70

        if isSelected then
            local pulse = math.floor(math.abs(math.sin(time * 3)) * 100)
            local border = 5
            DrawBorder(renderer, x - border, y - border, w + border * 2, h + border * 2,
                border, 255, 255 - pulse, 100, 255)
            DrawRect(renderer, x, y, w, h, 60, 60, 20 + pulse, 255)
        else
            DrawRect(renderer, x, y, w, h, 0, 100, 0, 255)
        end
        DrawText(renderer, option, x + 20, y + 20, 255, 255, 255, 255)
    end
end
