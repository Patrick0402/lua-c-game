--main.lua

-- Evitar avisos do editor
Input = Input or {}
DrawRect = DrawRect or function(...) end

-- Inicializar tabelas globais
Projectiles = Projectiles or {}
Enemies = Enemies or {}
EnemyProjectiles = EnemyProjectiles or {}

-- Temporizador de spawn de inimigos
local spawn_timer = 0
local spawn_interval = 2
local spawn_positions = {100,200,300,400,500,600,700}

-- Temporizadores de dificuldade
local difficulty_timer = 0
local difficulty_interval = 10

function AutoSpawnEnemy()
    -- Escolhe uma posição aleatória no eixo X
    local x = spawn_positions[math.random(#spawn_positions)]
    local y = -30 -- começa fora da tela (topo)
    SpawnEnemy(x, y)
end

-- Função de atualização do jogo
function UpdateGame(dt)
    -- Atualizar jogador
    Player:update(dt, Input)

    -- Atualizar projéteis
    for i = #Projectiles, 1, -1 do
        local p = Projectiles[i]
        p.y = p.y - p.speed * dt

        -- Verificar colisão com inimigos
        for j = #Enemies, 1, -1 do
            local e = Enemies[j]
            if p.x + p.w > e.x and p.x < e.x + e.w and
                p.y + p.h > e.y and p.y < e.y + e.h then
                -- Colisão: remover projétil e inimigo
                table.remove(Projectiles, i)
                table.remove(Enemies, j)
                break
            end
        end

        -- Remover projétil que sai da tela
        if p.y < 0 then
            table.remove(Projectiles, i)
        end
    end

    -- Atualizar inimigos
    UpdateEnemies(dt)
    UpdateEnemyProjectiles(dt)

    -- Atualizar temporizador de spawn
    spawn_timer = spawn_timer - dt
    if spawn_timer <= 0 then
        AutoSpawnEnemy()
        spawn_timer = spawn_interval
    end

    -- Aumentar dificuldade com o tempo
    difficulty_timer = difficulty_timer + dt
    if difficulty_timer > difficulty_interval then
        for _, e in ipairs(Enemies) do
            e.vx = e.vx * 1.1
            e.vy = e.vy * 1.1
        end
        difficulty_timer = 0
        if spawn_interval > 0.5 then
            spawn_interval = spawn_interval - 0.2 -- inimigos aparecem mais rápido
        end
    end
end

-- Funções de renderização
function DrawPlayer(renderer)
    local p = Player
    DrawRect(renderer, math.floor(p.x), math.floor(p.y), p.w, p.h, p.color.r, p.color.g, p.color.b, p.color.a)
end

function DrawEnemies(renderer)
    for _, e in ipairs(Enemies) do
        DrawRect(renderer, math.floor(e.x), math.floor(e.y), e.w, e.h, 255, 0, 0, 255)
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
