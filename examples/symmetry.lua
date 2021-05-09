-- Symmetry 0/2/2/M (Jason Rampe)
local rules = {
	states = 10,
	survives = {},
	born = {}
}

rules.born[2] = 1

local maps = {
	process = {},
	current = {}
}

local size = 20
local cellUpdateRate = 3/2

function clampWrapAround(val, lo, hi) -- Clamp wrap around
	if val > hi then
		return lo
	elseif val < lo then
		return hi
	else
		return val
	end
end

function mooreNeighbors(cx, cy, cz, gridSize)
	local neighbors = 0
	for x=cx-1, cx+1 do
		for y=cy-1, cy+1 do
			for z=cz-1, cz+1 do
				if (not (x==cx and y==cy and z==cz)) then
					local cux = clampWrapAround(x, 0, gridSize)
					local cuy = clampWrapAround(y, 0, gridSize)
					local cuz = clampWrapAround(z, 0, gridSize)
					if maps.current[cux][cuy][cuz].state ~= 0 then
						neighbors = neighbors + 1
					end
				end
			end
		end
	end
	return neighbors
end

function ca_calc(x, y, z)
	local neighbors = mooreNeighbors(x, y, z, size)
	local currentState = maps.current[x][y][z].state
	local newState = currentState
	
	if (currentState == 0) then
		if (rules.born[neighbors]) then
			newState = rules.states - 1
		end
	elseif (currentState == rules.states - 1) then
		if (rules.survives[neighbors] == nil) then
			newState = currentState - 1
		end
	else
		newState = currentState - 1
	end
	return newState
end

function setup()
	for x=0, size do
		maps.process[x] = {}
		maps.current[x] = {}
		for y=0, size do
			maps.process[x][y] = {}
			maps.current[x][y] = {}
			for z=0, size do
				local r, g, b = color(x, y, z, size)
				maps.process[x][y][z] = {
					state = 0,
					r = r,
					g = g,
					b = b
				}
				maps.current[x][y][z] = {
					state = 0,
					r = r,
					g = g,
					b = b
				}
			end
		end
	end
	maps.current[math.floor(size/2)][math.floor(size/2)][math.floor(size/2)].state = rules.states-1
	maps.current[math.floor(size/2)+1][math.floor(size/2)+1][math.floor(size/2+1)].state = rules.states-1
	return size+1, maps.current, cellUpdateRate
end

function update()
	for x=0, size do
		for y=0, size do
			for z=0, size do
				maps.process[x][y][z].state = ca_calc(x, y, z)
			end
		end
	end
	
	maps.current, maps.process = maps.process, maps.current
	return maps.current
end

function color(x, y, z, size)
	return 255*x/size, 255*y/size, 255*z/size
end
