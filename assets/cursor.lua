local moveSpeed = 10

local function softSign(v)
    if v > 0.3 then
        return 1
    end
    if v < -0.3 then
        return -1
    end
    return 0
end

function NewCursor()
    local self = {
        x = screen.width / 2,
        y = screen.height / 2,
        speedX = 10,
        speedY = 10,
        radius = 5,
        nx = 0,
        ny = 0,
    }

    local update = function(dt)
        self.x = self.x + self.speedX * mouse.dx * dt
        self.y = self.y + self.speedY * mouse.dy * dt
        self.nx = (self.x / screen.width - 0.5) * 2.0
        self.ny = (0.5 - self.y / screen.height) * 2.0

        if softSign(self.nx) ~= 0 then
            camera.moveRight(moveSpeed * self.nx * dt)
        end
        if softSign(self.ny) ~= 0 then
            camera.moveUp(moveSpeed * self.ny * dt)
        end

        self.x = self.x - 10 * moveSpeed * softSign(self.nx) * dt
        self.y = self.y + 10 * moveSpeed * softSign(self.ny) * dt
    end

    local getPos = function()
        return self.x, self.y
    end

    local draw = function()
        circle(self.x, self.y, self.radius)
    end

    return {
        draw = draw,
        update = update,
        getPos = getPos
    }
end

return NewCursor
