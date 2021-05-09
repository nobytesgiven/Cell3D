-- CA_3D template
-- For examples look at examples/

-- Initialize two tables for processing and displaying the CA grid
local maps = {
	process = {},
	current = {}
}

local size = 15 -- Cube 15x15x15
local cellUpdateRate = 1/3 -- 3 times per second

function setup()
	-- Fill tables with data
	for x=0, size do
		maps.process[x] = {}
		maps.current[x] = {}
		for y=0, size do
			maps.process[x][y] = {}
			maps.current[x][y] = {}
			for z=0, size do
				-- Contents of process generally do not matter, since
				-- they should be overwritten in each update iteration.
				maps.process[x][y][z] = {
					state = 1,
					r = 255,
					g = 255,
					b = 255
				}
				maps.current[x][y][z] = {
					state = 1,
					r = 255,
					g = 255,
					b = 255
				}
			end
		end
	end

	-- setup() returns in order:
	-- 1) The 3D grid edge length
	-- 2) The current 3D grid which is to be displayed
	-- 3) The rate at which update() will be called
	return size, maps.current, cellUpdateRate
end

function update()
	-- All of your cellular automata code should ONLY make
	-- changes to maps.process by reading maps.current


	-- Switch pointers (fast copy table and keep 2 tables on memory)
	-- I do this to keep maps.current as the current cell grid
	-- and have a table on hand (its contents do not matter since they
	-- will get replaced on the next iteration)
	maps.current, maps.process = maps.process, maps.current
	
	-- update() only returns the CA grid that is to be displayed
	return maps.current
end
