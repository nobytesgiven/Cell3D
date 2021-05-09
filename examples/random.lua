-- Random state and colors on each update

current = {}

local size = 15
local cellUpdateRate = 1/3

function setup()
	for x=0, size do
		current[x] = {}
		for y=0, size do
			current[x][y] = {}
			for z=0, size do
				current[x][y][z] = {
					state = math.random(0,1),
					r = math.random(0,255),
					g = math.random(0,255),
					b = math.random(0,255)
				}
			end
		end
	end
	return size+1, current, cellUpdateRate
end

function update()
	for x=0, size do
		for y=0, size do
			for z=0, size do
				current[x][y][z] = {
					state = math.random(0,1),
					r = math.random(0,255),
					g = math.random(0,255),
					b = math.random(0,255)
				}
			end
		end
	end
	return current
end

