-- Crystal Growth 2: 1-2/1,3/5/N (Jason Rampe)
local rules = {
	states = 5,
	survives = {},
	born = {}
}

rules.survives[1] = 1
rules.survives[2] = 1

rules.born[1] = 1
rules.born[3] = 1

local maps = {
	process = {},
	current = {}
}

local size = 40
local cellUpdateRate = 1

function clampWrapAround(val, lo, hi) -- Clamp wrap around
	if val > hi then
		return lo
	elseif val < lo then
		return hi
	else
		return val
	end
end

cWa = clampWrapAround

function neumannNeighbors(cx, cy, cz, gridSize)
	local neighbors = 0
	t = {}
	t[1] = maps.current[cWa(cx-1, 0, size)][cy][cz].state
	t[2] = maps.current[cWa(cx+1, 0, size)][cy][cz].state
	t[3] = maps.current[cx][cWa(cy-1, 0, size)][cz].state
	t[4] = maps.current[cx][cWa(cy+1, 0, size)][cz].state
	t[5] = maps.current[cx][cy][cWa(cz-1, 0, size)].state
	t[6] = maps.current[cx][cy][cWa(cz+1, 0, size)].state
	
	for i=1, 6 do
		if (t[i] ~= 0 and t[i]) then
			neighbors = neighbors + 1
		end
	end
	
	return neighbors
end

function ca_calc(x, y, z)
	local neighbors = neumannNeighbors(x, y, z, size)
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
				maps.process[x][y][z] = {
					state = 0,
					r = 0,
					g = 0,
					b = 0
				}
				maps.current[x][y][z] = {
					state = 0,
					r = 255,
					g = 255,
					b = 0
				}
			end
		end
	end

	maps.current[math.floor(size/2)][math.floor(size/2)][math.floor(size/2)].state = rules.states-1
	return size+1, maps.current, cellUpdateRate
end

function update()
	for x=0, size do
		for y=0, size do
			for z=0, size do
				local state = ca_calc(x, y, z)
				maps.process[x][y][z].state = state
				local r, g, b = color(x, y, z, state)
				maps.process[x][y][z].r = r
				maps.process[x][y][z].g = g
				maps.process[x][y][z].b = b
				
			end
		end
	end
	
	maps.current, maps.process = maps.process, maps.current
	return maps.current
end

function color(x, y, z, state)
	if state > 3 then
		return 255, 255, 0
	else
		return 255, 0, 0
	end
end
