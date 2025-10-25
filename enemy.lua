-- Inicializar tabela global de inimigos
Enemies = Enemies or {}
EnemyProjectiles = EnemyProjectiles or {}

-- Spawn de inimigos
function SpawnEnemy(x, y)
    table.insert(Enemies, {
        x = x,
        y = y,
        w = 40,
        h = 30,
        vx = 50 + math.random(-20,20),   -- velocidade horizontal aleatória
        vy = 20,
        shoot_timer = math.random() * 2, -- tempo até próximo tiro
        alive = true,
        zigzag_phase = math.random() * math.pi
    })
end

-- Spawn de projéteis inimigos
function EnemyShoot(e)
    local pl = Player
    local dx = (pl.x + pl.w/2) - (e.x + e.w/2)
    local dy = (pl.y + pl.h/2) - (e.y + e.h/2)
    local length = math.sqrt(dx*dx + dy*dy)
    dx = dx / length
    dy = dy / length

    table.insert(EnemyProjectiles, {
        x = e.x + e.w / 2 - 4,
        y = e.y + e.h,
        dx = dx,
        dy = dy,
        speed = 300 + math.random()*100,
        w = 8,
        h = 16
    })
end

-- Atualizar inimigos
function UpdateEnemies(dt)
    for i = #Enemies, 1, -1 do
        local e = Enemies[i]

        -- Movimento complexo: combinação de senos e cossenos para zigzag imprevisível
        e.zigzag_phase = e.zigzag_phase + dt * (1 + math.random()) * 3
        e.x = e.x + e.vx * dt * math.sin(e.zigzag_phase * 0.5) + math.cos(e.zigzag_phase) * 10 * dt
        e.y = e.y + e.vy * dt * (0.5 + math.random()) -- velocidade vertical variável

        -- Rebote horizontal
        if e.x < 0 then e.x = 0 e.vx = -e.vx end
        if e.x + e.w > Config.screen_width then e.x = Config.screen_width - e.w e.vx = -e.vx end

        -- Tiro inimigo
        e.shoot_timer = e.shoot_timer - dt
        if e.shoot_timer <= 0 then
            EnemyShoot(e)
            e.shoot_timer = 0.5 + math.random() * 1.5 -- tiros mais frequentes
        end

        -- Remover inimigos que saem da tela
        if e.y > Config.screen_height then
            table.remove(Enemies, i)
        end
    end
end


-- Atualizar projéteis inimigos
function UpdateEnemyProjectiles(dt)
    for i = #EnemyProjectiles, 1, -1 do
        local p = EnemyProjectiles[i]
        p.x = p.x + p.dx * p.speed * dt
        p.y = p.y + p.dy * p.speed * dt

        -- Checar colisão com jogador
        local pl = Player
        if p.x + p.w > pl.x and p.x < pl.x + pl.w and
           p.y + p.h > pl.y and p.y < pl.y + pl.h then
            -- Jogador atingido: reiniciar ou perder o jogo
            print("Você perdeu!")
            os.exit() -- fecha o jogo
        end

        if p.y > Config.screen_height then
            table.remove(EnemyProjectiles, i)
        end
    end
end

-- Expor globals
_G.Enemies = Enemies
_G.EnemyProjectiles = EnemyProjectiles
