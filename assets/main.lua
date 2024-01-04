print("Hello world!")

local timer = 0

MODEL = {
    ROCK_01=1,
    ROCK_02=2,
    ROCK_03=3,
    ROCK_04=4,
    ROCK_05=5,
    ROCK_13=6,
    ROCK_20=7,
    ROCK_21=8,
}

ROCKS = {}

function Rock(x, y, z, model)
    local self = {
        x=0,y=0,z=0,
        rot_x=1,rot_y=0,rot_z=0,
        rot_angle=0,
        model=model
    }

    local update = function(dt)
        self.rot_angle = self.rot_angle + dt * 100
    end

    local draw = function()
        mdlex(self.model, self.x, self.y, self.z, 1, self.rot_x, self.rot_y, self.rot_z, self.rot_angle)
    end

    return {
        update = update,
        draw = draw
    }
end


function Init()
    rock = Rock(0, 0, 0, MODEL.ROCK_03)
end

function Update()
    timer = timer + dt
    rock.update(dt)
end

function Draw()
    -- mdlex(1, 0,0,0, 1, 1,0,0, timer * 100)
    rock.draw()
end
