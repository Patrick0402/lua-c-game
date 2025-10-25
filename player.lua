-- player.lua

-- Inicializar Player
local Player = {
    x = 400, y = 500,
    w = 50, h = 50,
    speed = 300,
    color = {r=0, g=255, b=0, a=255},
    dx = 0, dy = 0,
    can_shoot = true,
    shoot_cooldown = 0.3,
    timer = 0
}

function Player:update(dt, input)
    -- Horizontal
    self.dx = 0
    if input.left then self.dx = -1 end
    if input.right then self.dx = 1 end
    self.x = self.x + self.dx * self.speed * dt
    self.x = math.max(0, math.min(self.x, Config.screen_width - self.w))

    -- Vertical
    self.dy = 0
    if input.up then self.dy = -1 end
    if input.down then self.dy = 1 end
    self.y = self.y + self.dy * self.speed * dt
    self.y = math.max(0, math.min(self.y, Config.screen_height - self.h))

    -- Atirar
    self.timer = self.timer - dt
    if input.shoot and self.timer <= 0 then
        self:shoot()
        self.timer = self.shoot_cooldown
    end
end


function Player:shoot()
    table.insert(Projectiles, {
        x = self.x + self.w / 2 - 4,
        y = self.y,
        dx = 0,
        dy = -1,
        speed = 800,
        w = 8,
        h = 16
    })
end

_G.Player = Player  -- exporta como global
