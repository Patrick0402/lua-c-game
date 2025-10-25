-- projectile.lua

function UpdateProjectiles(dt)
    for i = #Projectiles, 1, -1 do
        local p = Projectiles[i]
        p.x = p.x + p.dx * p.speed * dt
        p.y = p.y + p.dy * p.speed * dt
        if p.y < 0 then table.remove(Projectiles, i) end
    end
end

_G.Projectiles = Projectiles  -- exporta como global

