-- ==== Custom function ====

-- Return the result of "cd <dir> && ls <mask>", or nil if no file found
function ls_file(dir, mask)
	local out = spawner.popen(props['ndls.sh'] .. " 'cd \"" .. dir .. "\" ^&^& ls " .. mask .. "'")
	for line in out:lines() do
		if string.find(line, "No such file") then
			return nil
		else
			return line
		end
	end
end

-- Works without sh
function folder_exists(strFolderName)
	local fileHandle, strError = io.open(strFolderName.."\\*.*","r")
	if fileHandle ~= nil then
		io.close(fileHandle)
		return true
	else
		if string.match(strError,"No such file or directory") then
			return false
		else
			return true
		end
	end
end

-- =========================

if not folder_exists(props['SciteDefaultHome'] .. '/yagarto') or not folder_exists(props['SciteDefaultHome'] .. '/mingw-get') then
	print "WARNING: MSYS and YAGARTO components are missing, some commands won't work. Get them from http://ndlessly.wordpress.com/ ."
end
