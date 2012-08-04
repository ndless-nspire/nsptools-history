local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

function makeall()
	-- ^&: Windows escape
	local out = spawner.popen(props['ndls.sh'] .. " '" .. props['ndls.path'] .. " ^&^& cd \"" .. props['FileDir'] .. "\" ^&^& make'")
	for line in out:lines() do
		print(line)
	end
	print("Built.")
end

local file = io.open(props['FileDir'] .. "/Makefile", "r")
if not file then
	print("Please choose a name for the program.")
	scite.StripShow("!'Target program name:'[]((OK))(&Cancel)")
else
	io.close(file)
	makeall()
end

function OnStrip(control, change)
	if control == 2 and change == 1  --[[ OK clicked --]] then
		scite.StripShow("") -- hide
		local out = spawner.popen(props['ndls.sh'] .. " \" cd '" .. props['FileDir'] .. "' && '" .. props['SciteDefaultHome'] .. "/ndless/bin/nspire-tools' new '" .. scite.StripValue(1) .. "'\"")
		for line in out:lines() do
			print(line)
		end
		makeall()
	end
end
