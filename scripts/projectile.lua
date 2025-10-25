Projectiles = {}
EnemyProjectiles = {}

function SpawnProjectile(x, y)
    table.insert(Projectiles, { x = x, y = y, w = 4, h = 10, speed = 750 })
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
        local proj = EnemyProjectiles[i]
        proj.x = proj.x + proj.dx * proj.speed * dt
        proj.y = proj.y + proj.dy * proj.speed * dt

        local p = Player
        if p and proj.x < p.x + p.w and proj.x + proj.w > p.x and
           proj.y < p.y + p.h and proj.y + proj.h > p.y then

            if p.invulnerable_timer <= 0 then
                p.hp = p.hp - 1
                p.invulnerable_timer = 1.0 -- 1 segundo de imunidade
            end

            table.remove(EnemyProjectiles, i)

            if p.hp <= 0 then
                GameState.current = "gameover"
            end
        elseif proj.y > Config.screen_height then
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
