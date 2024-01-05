function NewShip()
    local self = {
        dx = 2,
        dy = -0.5
    }

    self.left = Rock(V3(-self.dx, self.dy, -7), V3(0, 0, 0), V3(0, 0, 0), MODEL.PUSHKA)
    self.right = Rock(V3(self.dx, self.dy, -7), V3(0, 0, 0), V3(0, 0, 0), MODEL.PUSHKA)

    local get = function()
        return self
    end

    local update = function(dt, camera)
        local pos1, pos2 = self.left.get().pos, self.right.get().pos

        pos1.x = camera.px - self.dx
        pos1.y = camera.py + self.dy
        pos2.x = camera.px + self.dx
        pos2.y = camera.py + self.dy
    end

    local draw = function()
        self.left.draw()
        self.right.draw()
    end

    return {
        get = get,
        update = update,
        draw = draw
    }
end
