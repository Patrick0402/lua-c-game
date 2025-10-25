Enemies = {}

function SpawnEnemy(x, y)
    table.insert(Enemies, {
        x = x, y = y,
        w = 40, h = 30,
        vx = 50 * GetSpeedMultiplier(),
        vy = 20 * GetSpeedMultiplier(),
        shoot_timer = math.random(1, 3)  -- tempo até o próximo tiro
    })
end

function ResetEnemySystem()
    Enemies = {}
end

function UpdateEnemies(dt)
    for _, e in ipairs(Enemies) do
        e.x = e.x + e.vx * dt
        e.y = e.y + e.vy * dt

        if e.x < 0 then
            e.x = 0; e.vx = -e.vx
        elseif e.x + e.w > Config.screen_width then
            e.x = Config.screen_width - e.w; e.vx = -e.vx
        end

        -- Disparo inimigo
        e.shoot_timer = e.shoot_timer - dt
        if e.shoot_timer <= 0 then
            table.insert(EnemyProjectiles, {
                x = e.x + e.w/2 - 2,
                y = e.y + e.h,
                w = 4,
                h = 10,
                speed = 200
            })
            e.shoot_timer = math.random(1, 3)  -- reinicia o timer
        end

        -- Remove se sair da tela
        if e.y > Config.screen_height then
            e.remove = true
        end
    end

    for i = #Enemies, 1, -1 do
        if Enemies[i].remove then table.remove(Enemies, i) end
    end
end

function DrawEnemies(renderer)
    for _, e in ipairs(Enemies) do
        DrawRect(renderer, math.floor(e.x), math.floor(e.y), e.w, e.h, 255, 0, 0, 255)
    end
end
