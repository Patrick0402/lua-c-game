Player = {
    x = 400,
    y = 550,
    w = 40,
    h = 30,
    speed = 200,
    hp = 3,
    color = { r = 0, g = 255, b = 255, a = 255 },
    cooldown = 0.2,
    shoot_timer = 0
}

function Player:update(dt, input)
    -- Movimento horizontal
    if input.left then self.x = self.x - self.speed * dt end
    if input.right then self.x = self.x + self.speed * dt end

    -- Movimento vertical
    if input.up then self.y = self.y - self.speed * dt end
    if input.down then self.y = self.y + self.speed * dt end

    -- Limites da tela
    self.x = math.max(0, math.min(self.x, Config.screen_width - self.w))
    self.y = math.max(0, math.min(self.y, Config.screen_height - self.h))

    -- Tiro
    self.shoot_timer = self.shoot_timer - dt
    if input.shoot and self.shoot_timer <= 0 then
        SpawnProjectile(self.x + self.w / 2 - 2, self.y - 10)
        self.shoot_timer = self.cooldown
    end
end

function Player:reset()
    self.x = 400
    self.y = 550
    self.hp = 3
end

function DrawPlayer(renderer)
    local p = Player
    DrawRect(renderer, math.floor(p.x), math.floor(p.y), p.w, p.h,
             p.color.r, p.color.g, p.color.b, p.color.a)
end
