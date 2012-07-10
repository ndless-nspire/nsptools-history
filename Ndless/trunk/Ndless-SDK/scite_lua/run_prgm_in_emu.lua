-- TODO search tns, error if not found

local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

local out = spawner.popen(props['ndls.sh'] .. " 'ls \"" .. props['FileDir'] .. "/\"*.tns'")
local tns
for line in out:lines() do
	tns = line
	break
end
if string.find(tns, "No such file")	then
	print("You must first build the program with Tools > Build.")
else
	local au_spawner = spawner.new(props['SciteDefaultHome'] .. "\\autoit\\autoit3.exe \"" .. props['SciteDefaultHome'] .. "\\autoit\\scripts\\tx_run_prgm_in_emu.au3\" \"" .. tns .. "\"")
	au_spawner:run()
end
