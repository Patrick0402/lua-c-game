-- Limites máximos de velocidade
MAX_ENEMY_SPEED = 400
MAX_PROJECTILE_SPEED = 600

-- Controle de dificuldade
SpawnTimer = 0
SpawnInterval = 2
DifficultyTimer = 0
DifficultyInterval = 10
SpawnPositions = {100,200,300,400,500,600,700}
SpeedMultiplier = 1

function ResetDifficulty()
    SpawnTimer = 0
    SpawnInterval = 2
    DifficultyTimer = 0
    SpeedMultiplier = 1
end

function HandleSpawning(dt)
    SpawnTimer = SpawnTimer - dt
    if SpawnTimer <= 0 then
        local x = SpawnPositions[math.random(#SpawnPositions)]
        local y = -30
        SpawnEnemy(x, y)
        SpawnTimer = SpawnInterval
    end
end

function HandleDifficulty(dt)
    DifficultyTimer = DifficultyTimer + dt
    if DifficultyTimer > DifficultyInterval then
        DifficultyTimer = 0
        SpeedMultiplier = math.min(SpeedMultiplier * 1.1, MAX_ENEMY_SPEED / 50)
        SpawnInterval = math.max(SpawnInterval - 0.2, 0.5)

        -- aumenta velocidade dos projéteis já existentes
        for _, p in ipairs(EnemyProjectiles) do
            p.speed = math.min(p.speed * 1.1, MAX_PROJECTILE_SPEED)
        end
    end
end

function GetSpeedMultiplier()
    return SpeedMultiplier
end
