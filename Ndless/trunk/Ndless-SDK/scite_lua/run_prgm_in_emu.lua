-- TODO search tns, error if not found

local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

local out = spawner.popen(props['ndls.sh'] .. " 'cd \"" .. props['FileDir'] .. "\" ^&^& ls *.tns'")
local tns
for line in out:lines() do
	tns = line
	break
end
if string.find(tns, "No such file")	then
	print("You must first build the program with Tools > Build.")
else
	-- Launch the emu if not running
	local out = spawner.popen(props['SciteDefaultHome'] .. "\\autoit\\autoit3.exe \"" .. props['SciteDefaultHome'] .. "\\autoit\\scripts\\is_emu_running.au3\"")
	for line in out:lines() do
		if line == "NO" then
			dofile(props['SciteDefaultHome'] .. "/scite_lua/run_emu.lua")
		end
		break
	end
	-- Run the program
	local au_spawner = spawner.new(props['SciteDefaultHome'] .. "\\autoit\\autoit3.exe \"" .. props['SciteDefaultHome'] .. "\\autoit\\scripts\\tx_run_prgm_in_emu.au3\" \"" .. props['FileDir'] .. "\\" .. tns .. "\"")
	au_spawner:run()
end
