Projectiles = {}
EnemyProjectiles = {}

function SpawnProjectile(x, y)
    table.insert(Projectiles, { x = x, y = y, w = 4, h = 10, speed = 400 })
end

function UpdateProjectiles(dt)
    for i = #Projectiles, 1, -1 do
        local p = Projectiles[i]
        p.y = p.y - p.speed * dt

        for j = #Enemies, 1, -1 do
            local e = Enemies[j]
            if p.x + p.w > e.x and p.x < e.x + e.w and
                p.y + p.h > e.y and p.y < e.y + e.h then
                table.remove(Projectiles, i)
                table.remove(Enemies, j)
                Score = Score + 100 -- <--- Aumenta 100 pontos
                break
            end
        end

        if p.y < 0 then
            table.remove(Projectiles, i)
        end
    end
end

function UpdateEnemyProjectiles(dt)
    for i = #EnemyProjectiles, 1, -1 do
        local p = EnemyProjectiles[i]
        p.y = p.y + p.speed * dt

        -- Verifica colisão com o Player
        if p.x + p.w > Player.x and p.x < Player.x + Player.w and
            p.y + p.h > Player.y and p.y < Player.y + Player.h then
            Player.hp = Player.hp - 1         -- Player perde 1 de HP
            table.remove(EnemyProjectiles, i) -- Remove o projétil
        elseif p.y > Config.screen_height then
            table.remove(EnemyProjectiles, i)
        end
    end
end

function DrawProjectiles(renderer)
    for _, p in ipairs(Projectiles) do
        DrawRect(renderer, math.floor(p.x), math.floor(p.y), p.w, p.h, 255, 255, 0, 255)
    end
end

function DrawEnemyProjectiles(renderer)
    for _, p in ipairs(EnemyProjectiles) do
        DrawRect(renderer, math.floor(p.x), math.floor(p.y), p.w, p.h, 255, 0, 255, 255)
    end
end
