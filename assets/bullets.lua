function NewShooter()
    local self = {
        is_left = false
    }

    local spawn = function(cursor)
        local bpos = Ship.get().left.get().pos
        if self.is_left then
            bpos = Ship.get().right.get().pos
        end

        self.is_left = not self.is_left
        local ray = ray(cursor.getPos())

        local dz = V3(0, 0, 0)
        local spawn = V3Add(bpos, dz)
        local speed = V3Mul(V3Norm(ray.dir), 30)

        table.insert(BULLETS, Bullet(spawn, speed))
    end

    return {
        spawn = spawn
    }
end

function Bullet(pos, speed)
    local self = {
        pos = pos,
        speed = speed,
        nspeed = V3Mul(V3Norm(speed), 2)
    }

    local get = function()
        return self
    end

    local update = function(dt)
        self.pos = V3Add(self.pos, V3Mul(self.speed, dt))
    end

    local draw = function()
        -- star(self.pos.x, self.pos.y, self.pos.z, 1)
        local p1 = V3Add(self.pos, self.nspeed)
        line3d(self.pos.x, self.pos.y, self.pos.z, p1.x, p1.y, p1.z)
    end

    return {
        get = get,
        update = update,
        draw = draw
    }
end
