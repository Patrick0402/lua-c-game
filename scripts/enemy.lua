Enemies = {}

function ResetEnemySystem()
    Enemies = {}
end

function SpawnEnemy(x, y)
    table.insert(Enemies, {
        x = x,
        y = y,
        w = 40,
        h = 30,
        vx = 50 * GetSpeedMultiplier(),
        vy = 20 * GetSpeedMultiplier(),
        shoot_timer = math.random(1, 3),
        zigzag_phase = math.random() * math.pi
    })
end

function UpdateEnemies(dt)
    for i = #Enemies, 1, -1 do
        local e = Enemies[i]

        -- Movimento
        e.zigzag_phase = e.zigzag_phase + dt * 3
        e.x = e.x + e.vx * dt * math.sin(e.zigzag_phase)
        e.y = e.y + e.vy * dt

        -- Rebote horizontal
        if e.x < 0 then
            e.x = 0
            e.vx = -e.vx
        end
        if e.x + e.w > Config.screen_width then
            e.x = Config.screen_width - e.w
            e.vx = -e.vx
        end

        -- 🧱 Colisão com o jogador
        local p = Player
        if e.x < p.x + p.w and
            e.x + e.w > p.x and
            e.y < p.y + p.h and
            e.y + e.h > p.y then
            -- Dano
            if p.invulnerable_timer <= 0 then
                p.hp = p.hp - 1
                p.invulnerable_timer = 1.0 -- 1 segundo de invulnerabilidade
            end
            -- Remove inimigo que colidiu
            table.remove(Enemies, i)

            -- Se o HP chegar a 0, vai para gameover
            if p.hp <= 0 then
                GameState.current = "gameover"
            end
        end

        -- Tiro inimigo
        e.shoot_timer = e.shoot_timer - dt
        if e.shoot_timer <= 0 and Player then
            EnemyShoot(e)
            e.shoot_timer = 0.5 + math.random() * 2
        end

        -- Remover inimigos fora da tela
        if e.y > Config.screen_height then
            table.remove(Enemies, i)
        end
    end
end

function EnemyShoot(e)
    if not Player then return end
    local pl = Player
    local dx = (pl.x + pl.w / 2) - (e.x + e.w / 2)
    local dy = (pl.y + pl.h / 2) - (e.y + e.h / 2)
    local len = math.sqrt(dx * dx + dy * dy)
    if len == 0 then len = 1 end
    dx, dy = dx / len, dy / len

    local speed = math.min((250 + math.random() * 100) * GetSpeedMultiplier(), MAX_PROJECTILE_SPEED)

    table.insert(EnemyProjectiles, {
        x = e.x + e.w / 2 - 4,
        y = e.y + e.h,
        dx = dx,
        dy = dy,
        speed = speed,
        w = 8,
        h = 16
    })
end

function DrawEnemies(renderer)
    for _, e in ipairs(Enemies) do
        DrawRect(renderer, math.floor(e.x), math.floor(e.y), e.w, e.h, 255, 0, 0, 255)
    end
end
