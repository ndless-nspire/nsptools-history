--[[
Keys:
- R: restart
- Enter: dig
- F : set/unset flag/unknown

TODO:
- Check won
- Display keys
- Display gameover
- Use images
- Simplify code with classes
--]]

function drawCenteredString(gc, s, x, y, w, h)
	gc:drawString(s, x + (w - gc:getStringWidth(s)) / 2, y + h /2, "middle")
end


function createGrid()
	gameover = false
	width, height, minesnum = 9, 9, 10
	currentCell = {x = 5, y = 5}
	grid = {} -- width, height, flags. Starts at 1,1
	for	i=1,width	do -- empty grid
		grid[i]	=	{}
		for	j=1,height do
			grid[i][j] = {masked = true}
		end
	end
	for i=1,minesnum do -- set the mines
		local minex, miney 
		repeat
			minex, miney = math.random(1,width), math.random(1,height)
		until not grid[minex][miney].mine
		grid[minex][miney].mine = true
	end
end

function drawGrid(gc)
	if not grid then return end
	local cellsize = platform.window:height() / height
	local topx = (platform.window:width() - cellsize * width) / 2
	local topy = 0
	for i=1,width do
		for j=1,height do
			local x, y, w, h = topx + (i-1) * cellsize + 1, topy + (j-1) * cellsize + 1, cellsize - 2, cellsize - 2
			if grid[i][j].masked then
				gc:setColorRGB(0, 0, 0)
				gc:fillRect(x, y, w, h)
				if grid[i][j].flag or grid[i][j].unknown then
					if grid[i][j].flag then c = "F" else c = "?" end
					gc:setColorRGB(255, 255, 255)
					drawCenteredString(gc, c, topx + (i-1) * cellsize , topy + (j-1) * cellsize, cellsize, cellsize)
				end
			elseif grid[i][j].mine or grid[i][j].wrong then
				gc:setColorRGB(15*16, 15*16, 15*16)
				gc:fillRect(x, y, w, h)
				local c
				if grid[i][j].mine then c = "B" else c = "X" end
				gc:setColorRGB(0, 0, 0)
				drawCenteredString(gc, c, topx + (i-1) * cellsize , topy + (j-1) * cellsize, cellsize, cellsize)
			else -- unmasked
				gc:setColorRGB(13*16, 13*16, 13*16)
				gc:fillRect(x, y, w, h)
				if grid[i][j].minesNum and grid[i][j].minesNum ~= 0 then
					gc:setColorRGB(0, 0, 0)
					drawCenteredString(gc, grid[i][j].minesNum, topx + (i-1) * cellsize , topy + (j-1) * cellsize, cellsize, cellsize)
				end
			end
		end
	end
	gc:setColorRGB(255, 255, 255)
	gc:drawRect(topx + (currentCell.x-1) * cellsize + 3, topy + (currentCell.y-1) * cellsize + 3, cellsize - 6, cellsize - 6)
	gc:drawRect(topx + (currentCell.x-1) * cellsize + 4, topy + (currentCell.y-1) * cellsize + 4, cellsize - 8, cellsize - 8)
end

-- grid-safe
function onEachCell(startX, endX, startY, endY, closure) -- closure(x, y)
	for i=math.max(startX, 1), math.min(endX, width) do
		for j=math.max(startY, 1), math.min(endY, height) do
			closure(i, j)
		end
	end
end

function countMines(cell) -- cell.x, cell.y
	local minesNum = 0
	onEachCell(cell.x - 1, cell.x + 1, cell.y - 1, cell.y + 1, function(x, y)
		if grid[x][y].mine then minesNum = minesNum + 1 end
	end)
	return minesNum
end

function unmaskCell(cell) -- cell.x, cell.y. No mine there.
	grid[cell.x][cell.y] = {masked = false, emtpy = true}
	grid[cell.x][cell.y].minesNum = countMines(cell)
	print(cell.x, cell.y, ": ", grid[cell.x][cell.y].minesNum)
	if grid[cell.x][cell.y].minesNum == 0 then
		onEachCell(cell.x - 1, cell.x + 1, cell.y - 1, cell.y + 1, function(x, y)
			if grid[x][y].masked and not grid[x][y].mine and not grid[x][y].flag and not grid[x][y].unknown then
				unmaskCell({x = x, y = y})
			end
		end)
	end
end

function on.charIn(ch)
	if ch == "r" then
			createGrid()
	elseif ch == "f" then
		if grid[currentCell.x][currentCell.y].flag then
			grid[currentCell.x][currentCell.y].flag = false
			grid[currentCell.x][currentCell.y].unknown = true
		elseif grid[currentCell.x][currentCell.y].unknown then
			grid[currentCell.x][currentCell.y].unknown = false
		else
			grid[currentCell.x][currentCell.y].flag = true
		end
	end
	platform.window:invalidate()
end

on.charIn("r")

function on.paint(gc)
	drawGrid(gc)
	if grid[currentCell.x][currentCell.y].minesNum then
		gc:drawString(grid[currentCell.x][currentCell.y].minesNum, 0, 0)
	end
end

function on.arrowKey(key)
	if gameover then return end
	if key == "left" and currentCell.x > 1 then currentCell.x = currentCell.x - 1
	elseif key == "right" and currentCell.x < width then currentCell.x = currentCell.x + 1
	elseif key == "up" and currentCell.y > 1 then currentCell.y = currentCell.y - 1
	elseif key == "down" and currentCell.y < height then currentCell.y = currentCell.y + 1
	end
	platform.window:invalidate()
end

function on.enterKey()
	if gameover then return end
	if grid[currentCell.x][currentCell.y].flag or grid[currentCell.x][currentCell.y].unknown then return end
	if grid[currentCell.x][currentCell.y].mine then
		gameover = true
		for	i=1,width	do -- display the full grid
			for	j=1,height do
				grid[i][j].masked = false
				if grid[i][j].flag and not grid[i][j].mine then grid[i][j].wrong = true end
			end
		end
	else
		unmaskCell(currentCell)
	end
	platform.window:invalidate()
end
