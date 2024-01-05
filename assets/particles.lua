function NewParticle(pos, speed, model)
    local self = {
        pos = pos,
        speed = speed,
        model = model,
        timer = 0,
        effect_speed = 10,
        effect_total_time = 3
    }

    local done = function()
        return self.timer >= self.effect_total_time
    end

    local update = function(dt)
        self.timer = self.timer + dt
        self.pos = V3Add(self.pos, V3Mul(self.speed, dt))
        -- TODO: add rotation
        -- self.rot_angle = self.rot_angle + dt * 100
    end

    local draw = function()
        ptcl(model, self.pos.x, self.pos.y, self.pos.z, self.timer, self.effect_speed, self.effect_total_time)
    end

    return {
        done = done,
        update = update,
        draw = draw
    }
end
