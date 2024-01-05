require("assets/bullets")
require("assets/objects")
require("assets/ship")
require("assets/vectors")
require("assets/cursor")

local cursor = NewCursor()
local timer = 0

MODEL = {
    ROCK_01 = 1,
    ROCK_02 = 2,
    ROCK_03 = 3,
    ROCK_04 = 4,
    ROCK_05 = 5,
    ROCK_13 = 6,
    ROCK_20 = 7,
    ROCK_21 = 8,
    PUSHKA = 9
}

function randomPos()
    local scale = 15
    return (rand() - 0.5) * scale, (rand() - 0.5) * scale
end

function generateRandomStar()
    local angle = rand() * 2 * math.pi
    local minDistanca, maxDistance = 15, 75
    local distance = rand(minDistanca, maxDistance)
    local x, y = math.cos(angle), math.sin(angle)
    x, y = x * distance, y * distance
    local scale = 0.5
    local speedMin, speedMax = -10, -5
    local initDist = 150
    local radius = rand() * scale
    local speed = V3(0, 0, rand(speedMin, speedMax))
    table.insert(ROCKS, Star(V3(x, y, initDist), radius, speed))
end

function generateRandomRock()
    local initDist = 150
    local x, y = randomPos()
    local minusHalf = V3(-0.5, -0.5, -0.5)
    local rotation = V3Add(V3Rand(), minusHalf)
    local speed = V3(0, 0, -10)
    local model = rand(1, 8)
    table.insert(ROCKS, Rock(V3(x, y, initDist), rotation, speed, model))
end

function updateRocks()
    for i = #ROCKS, 1, -1 do
        local rock = ROCKS[i]
        local raw = rock.get()
        rock.update(dt)
        if raw.pos.z < -10 then
            table.remove(ROCKS, i)
            goto continue
        end

        if not raw.collide then
            goto continue
        end

        -- int i, float px, float py, float pz, float rx, float ry, float rz, float angle,
        -- float scale, float lx1, float ly1, float lz1, float lx2, float ly2, float lz2
        for j = #BULLETS, 1, -1 do
            local bullet = BULLETS[j].get()
            local l1, l2 = bullet.pos, bullet.nspeed
            local mpos = raw.pos
            local mrot = raw.rot
            local res = collmdlline(raw.model, mpos.x, mpos.y, mpos.z, mrot.x, mrot.y, mrot.z, raw.rot_angle,
                1, l1.x, l1.y, l1.z, l2.x, l2.y, l2.z)

            if res.hit then
                table.remove(BULLETS, j)
                table.remove(ROCKS, i)
                goto continue
            end
        end
        ::continue::
    end
end

ROCKS = {}
TIMERS = {}
BULLETS = {}

function Init()
    for i = 1, 100 do
        generateRandomStar()
    end

    Ship = NewShip()
    Shooter = NewShooter()

    table.insert(TIMERS, Timer(0.02, generateRandomStar))
    table.insert(TIMERS, Timer(1, generateRandomRock))
end

function Update()
    timer = timer + dt

    updateRocks()

    for i = 1, #TIMERS do
        TIMERS[i].update(dt)
    end

    for i = #BULLETS, 1, -1 do
        BULLETS[i].update(dt)
        if BULLETS[i].get().pos.z > 100 then
            table.remove(BULLETS, i)
        end
    end

    cursor.update(dt)
    Ship.update(dt, camera)

    if ismouse() then
        Shooter.spawn(cursor)
    end
end

function Draw()
    for i = 1, #ROCKS do
        ROCKS[i].draw()
    end

    for i = 1, #BULLETS do
        BULLETS[i].draw()
    end

    Ship.draw()
end

function DrawCanvas()
    cursor.draw()
end
