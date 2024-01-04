function Rock(pos, rot, speed, model)
    local self = {
        pos=pos,
        rot=rot,
        speed=speed,
        rot_angle=0,
        model=model
    }

    local get = function()
        return self
    end

    local update = function(dt)
        self.pos = V3Add(self.pos, V3Mul(self.speed, dt))
        self.rot_angle = self.rot_angle + dt * 100
    end

    local draw = function()
        mdlex(self.model, self.pos.x, self.pos.y, self.pos.z, 1, self.rot.x, self.rot.y, self.rot.z, self.rot_angle)
    end

    return {
        get=get,
        update = update,
        draw = draw
    }
end

function Star(pos, radius, speed)
    local self = {
        pos=pos,
        speed=speed,
        radius=radius
    }

    local get = function()
        return self
    end

    local update = function(dt)
        self.pos = V3Add(self.pos, V3Mul(self.speed, dt))
    end

    local draw = function()
        star(self.pos.x, self.pos.y, self.pos.z, self.radius)
    end

    return {
        get = get,
        update = update,
        draw = draw
    }
end

function Timer(period, action)
    local self = {
        period = period,
        action = action,
        time = 0
    }

    function update(dt)
        self.time = self.time + dt
        if self.time > self.period then
            self.time = 0
            self.action()
        end
    end

    return {
        update = update
    }
end