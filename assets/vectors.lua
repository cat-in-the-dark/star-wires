rand = math.random

function V3( x,y,z )
    local y = y or x
    local z = z or x
    return {x=x,y=y,z=z}
end

function V3Add( v1,v2 )
    return {x=v1.x+v2.x,y=v1.y+v2.y,z=v1.z+v2.z}
end

function V3Mul( v1,n )
    return {x=v1.x*n,y=v1.y*n,z=v1.z*n,h=v1.h}
end

function V3Rand()
    return {x = rand(), y = rand(), z = rand()}
end