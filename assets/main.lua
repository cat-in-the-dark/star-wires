require("assets/objects")
require("assets/vectors")
require("assets/cursor")

local cursor = NewCursor()
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

function randomPos()
    local scale = 15
    return (rand() - 0.5) * scale, (rand() - 0.5) * scale
end

function generateRandomStar()
    local angle = rand() * 2 * math.pi
    local minDistanca, maxDistance = 15, 100
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
    local speed = V3(0, 0, -20)
    local model = rand(1, 8)
    table.insert(ROCKS, Rock(V3(x, y, initDist), rotation, speed, model))
end

function updateRocks()
    for i=#ROCKS,1,-1 do
        ROCKS[i].update(dt)
        if ROCKS[i].get().pos.z < -10 then
            table.remove(ROCKS, i)
        end
    end
end

ROCKS = {}
TIMERS = {}

function Init()
    for i=1,100 do
        generateRandomStar()
    end

    table.insert(TIMERS, Timer(0.02, generateRandomStar))
    table.insert(TIMERS, Timer(0.5, generateRandomRock))
end

function Update()
    print(#ROCKS)
    timer = timer + dt

    updateRocks()

    for i=1,#TIMERS do
        TIMERS[i].update(dt)
    end

    cursor.update(dt)
end

function Draw()
    for i=1,#ROCKS do
        ROCKS[i].draw()
    end
end

function DrawCanvas()
    cursor.draw()
end