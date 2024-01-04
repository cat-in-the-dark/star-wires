print("Hello world!")

local timer = 0

function Update()
    timer = timer + dt
end

function Draw()
    mdlex(1, 0,0,0, 1, 1,0,0, timer * 100)
end
