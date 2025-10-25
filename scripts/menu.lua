MenuOptions = { "Start", "Exit" }
MenuSelected = 1
local shoot_was_pressed = false

DrawTextCenter = DrawTextCenter or function(...) end


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
    local screenWidth = Config.screen_width or 800
    local screenHeight = Config.screen_height or 600

    -- Fundo
    DrawRect(renderer, 0, 0, screenWidth, screenHeight, 10, 10, 40, 255)

    -- Título
    local title = "SPACE INVADERS LUA"
    local titleX = screenWidth / 2
    local titleY = 120
    DrawTextCenter(renderer, title, titleX, titleY, 255, 255, 255, 255)

    -- Botões do menu
    local buttonWidth = 300
    local buttonHeight = 80
    local startY = 260
    local spacing = 100
    local time = os.clock()

    for i, option in ipairs(MenuOptions) do
        local isSelected = (i == MenuSelected)
        local x = screenWidth / 2 - buttonWidth / 2
        local y = startY + (i - 1) * spacing

        if isSelected then
            local pulse = math.floor(math.abs(math.sin(time * 3)) * 100)
            local border = 6
            DrawBorder(renderer, x - border, y - border, buttonWidth + border * 2, buttonHeight + border * 2,
                border, 255, 255 - pulse, 100, 255)
            DrawRect(renderer, x, y, buttonWidth, buttonHeight, 60, 60, 20 + pulse, 255)
        else
            DrawRect(renderer, x, y, buttonWidth, buttonHeight, 0, 100, 0, 255)
        end

        -- Centraliza texto do botão
        local textX = x + buttonWidth / 2 
        local textY = y + buttonHeight / 2
        DrawTextCenter(renderer, option, textX, textY, 255, 255, 255, 255)
    end
end

function UpdateGameOverMenu(input)
    if input.up_pressed then
        GameOverSelected = GameOverSelected - 1
        if GameOverSelected < 1 then GameOverSelected = #GameOverOptions end
    elseif input.down_pressed then
        GameOverSelected = GameOverSelected + 1
        if GameOverSelected > #GameOverOptions then GameOverSelected = 1 end
    end

    if input.shoot and not GameOverShootWasPressed then
        GameOverShootWasPressed = true
        local choice = GameOverOptions[GameOverSelected]
        if choice == "Jogar novamente" then
            GameState.current = "playing"
            ResetGame()
        elseif choice == "Menu principal" then
            GameState.current = "menu"
        end
    elseif not input.shoot then
        GameOverShootWasPressed = false
    end
end

function DrawGameOverMenu(renderer)
    local screenWidth = Config.screen_width or 800
    local screenHeight = Config.screen_height or 600

    -- Fundo
    DrawRect(renderer, 0, 0, screenWidth, screenHeight, 50, 0, 0, 255)

    -- Título
    local title = "GAME OVER"
    local titleX = screenWidth / 2
    local titleY = 120
    DrawTextCenter(renderer, title, titleX, titleY, 255, 255, 255, 255)

    -- Pontuação
    local scoreText = "Score final: " .. Score
    local scoreX = screenWidth / 2
    local scoreY = 180
    DrawTextCenter(renderer, scoreText, scoreX, scoreY, 255, 255, 0, 255)

    -- Botões
    local buttonWidth = 300
    local buttonHeight = 80
    local startY = 250
    local spacing = 100
    local time = os.clock()

    for i, option in ipairs(GameOverOptions) do
        local isSelected = (i == GameOverSelected)
        local x = screenWidth / 2 - buttonWidth / 2
        local y = startY + (i - 1) * spacing

        if isSelected then
            local pulse = math.floor(math.abs(math.sin(time * 3)) * 100)
            local border = 6
            DrawBorder(renderer, x - border, y - border, buttonWidth + border * 2, buttonHeight + border * 2,
                border, 255, 255 - pulse, 100, 255)
            DrawRect(renderer, x, y, buttonWidth, buttonHeight, 60, 60, 20 + pulse, 255)
        else
            DrawRect(renderer, x, y, buttonWidth, buttonHeight, 0, 100, 0, 255)
        end

        -- Centraliza texto do botão
        local textX = x + buttonWidth / 2
        local textY = y + buttonHeight / 2
        DrawTextCenter(renderer, option, textX, textY, 255, 255, 255, 255)
    end
end

function DrawBorder(renderer, x, y, w, h, thickness, r, g, b, a)
    DrawRect(renderer, x, y, w, thickness, r, g, b, a)
    DrawRect(renderer, x, y + h - thickness, w, thickness, r, g, b, a)
    DrawRect(renderer, x, y, thickness, h, r, g, b, a)
    DrawRect(renderer, x + w - thickness, y, thickness, h, r, g, b, a)
end
