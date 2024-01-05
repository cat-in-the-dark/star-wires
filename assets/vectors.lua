rand = math.random

function V3(x, y, z)
    local y = y or x
    local z = z or x
    return { x = x, y = y, z = z }
end

function V3Add(v1, v2)
    return { x = v1.x + v2.x, y = v1.y + v2.y, z = v1.z + v2.z }
end

function V3Mul(v1, n)
    return { x = v1.x * n, y = v1.y * n, z = v1.z * n }
end

function V3Sub(v1, v2)
    return V3Add(v1, V3Mul(v2, -1))
end

function V3Rand()
    return { x = rand(), y = rand(), z = rand() }
end

function V3CrossProduct(v1, v2)
    return { x = v1.y * v2.z - v1.z * v2.y, y = v1.z * v2.x - v1.x * v2.z, z = v1.x * v2.y - v1.y * v2.x }
end

function V3Len(v)
    return math.sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
end

function V3Norm(v)
    local res = V3(0, 0, 0)
    local len = V3Len(v)

    if len ~= 0 then
        res = V3Mul(v, 1 / len)
    end

    return res
end

function V3Angle(v1, v2)
    local cross = { x = v1.y * v2.z - v1.z * v2.y, y = v1.z * v2.x - v1.x * v2.z, z = v1.x * v2.y - v1.y * v2.x };
    local len = math.sqrt(cross.x * cross.x + cross.y * cross.y + cross.z * cross.z);
    local dot = (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z);
    local result = math.atan(len, dot);

    return result;
end
