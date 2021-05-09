-- Convways game of life with 3D history

local grid = {}
local size = 20
cellUpdateRate = 1/3

local gridMiddleman = {}
local stackPosition = 0

setmetatable(gridMiddleman, {
	__index = function(t, x)
		return setmetatable({}, {
				__index = function(t, y)
					return setmetatable({}, {
							__index = function(t, z)
								return setmetatable({}, {
									__index = function(t, key)
									if key ~= "state" then
										if convertIndex(z) == convertIndex(size-1) then
											return 255
										else
											if key == "r" then
												return 255*z/size
											elseif key == "g" then
												return 255*x/size
											elseif key == "b" then
												return 255*y/size
											end
										end
									end
										return grid[convertIndex(z)][y][x][key]
									end
								})
							end
						})
				end
			})
	end
})

function wrapClamp(val, lo, hi)
	if val > hi then
		return lo + val - hi
	elseif val < lo then
		return hi + val - lo
	else
		return val
	end
end

function convertIndex(i)
	return wrapClamp(i-stackPosition, 0, size)
end

function CAWA(val, lo, hi)
	if val > hi then
		return lo
	elseif val < lo then
		return hi
	else
		return val
	end
end

function ca_calc(currentState, neighbors)
	if currentState == 1 then
		if neighbors < 2 then
			return 0
		elseif neighbors == 2 or neighbors == 3 then
			return 1
		else
			return 0
		end
	else
		if neighbors == 3 then
			return 1
		else
			return 0
		end
	end
end

function setup()
	for x=0, size-1 do
		grid[x] = {}
		for y=0, size-1 do
			grid[x][y] = {}
			for z=0, size-1 do
				local s = 0
				if x == convertIndex(size-1) then 
					s = math.random(0, 1)
				end
				grid[x][y][z] = {
					state = s,
					r = 255,
					g = 255,
					b = 255
				}
			end
		end
	end
	
	return size, gridMiddleman, cellUpdateRate
end

function update()
	stackPosition = wrapClamp(stackPosition-1, 0, size)
	
	for x=0, size-1 do
		for y=0, size-1 do
			local neighbors = 0
			for cx=x-1, x+1 do
				for cy=y-1, y+1 do
					if cx ~= x or cy ~= y then
						if grid[convertIndex(size-2)][CAWA(cx, 0, size-1)][CAWA(cy, 0, size-1)].state == 1 then
							neighbors = neighbors + 1
						end
					end
				end
			end
			local q = ca_calc(grid[convertIndex(size-2)][x][y].state, neighbors)	
			grid[convertIndex(size-1)][x][y].state = q
		end
	end
	
	return gridMiddleman
end
