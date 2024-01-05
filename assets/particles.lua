function NewParticle(pos, speed, rot, rot_angle, model)
    local self = {
        pos = pos,
        rot = rot,
        rot_angle = rot_angle,
        speed = speed,
        model = model,
        timer = 0,
        effect_speed = 5,
        effect_total_time = 1
    }

    local done = function()
        return self.timer >= self.effect_total_time
    end

    local update = function(dt)
        self.timer = self.timer + dt
        self.pos = V3Add(self.pos, V3Mul(self.speed, dt))
    end

    local draw = function()
        ptcl(model, self.pos.x, self.pos.y, self.pos.z, self.rot.x, self.rot.y, self.rot.z, self.rot_angle,
            self.timer, self.effect_speed, self.effect_total_time)
    end

    return {
        done = done,
        update = update,
        draw = draw
    }
end
