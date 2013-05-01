local clock = os.clock
function sleep(n) -- seconds
  local t0 = clock()
  while clock() - t0 <= n do end
end

local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

function get_tns_name()
	local out = spawner.popen(props['ndls.sh'] .. " 'cd \"" .. props['FileDir'] .. "\" ^&^& ls *.tns'")
	for line in out:lines() do
		if string.find(line, "No such file")	then
			return nil
		else
			return line
		end
	end
	
end

local tns = get_tns_name()
if tns == nil then
	dofile(props['SciteDefaultHome'] .. "/scite_lua/build.lua")
	for i = 0, 5 do
		sleep(1)
		tns = get_tns_name()
		if tns ~= nil then break end
	end
end
if tns ~= nil then
	-- Launch the emu if not running
	local out = spawner.popen(props['SciteDefaultHome'] .. "\\autoit\\autoit3.exe \"" .. props['SciteDefaultHome'] .. "\\autoit\\scripts\\is_emu_running.au3\"")
	for line in out:lines() do
		if line == "NO" then
			dofile(props['SciteDefaultHome'] .. "/scite_lua/run_emu.lua")
			sleep(14)
		end
		break
	end
	-- Run the program
	local au_spawner = spawner.new(props['SciteDefaultHome'] .. "\\autoit\\autoit3.exe \"" .. props['SciteDefaultHome'] .. "\\autoit\\scripts\\tx_run_prgm_in_emu.au3\" \"" .. props['FileDir'] .. "\\" .. tns .. "\"")
	au_spawner:run()
end
