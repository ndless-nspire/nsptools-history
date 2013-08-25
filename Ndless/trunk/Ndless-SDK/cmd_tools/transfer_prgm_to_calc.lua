local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

function navnetcmd_processchunk_cb(chunk)
	trace(chunk)
end

function navnetcmd(tns)
	local spawner = spawner.new("cmd /c \"cd " .. props['FileDir'] .. " & " .. props['SciteDefaultHome'] .. "\\cmd_tools\\navnetcmd\\navnetcmd.bat put " .. tns .. " /ndless/" .. tns .. "\"")
	spawner:set_output("navnetcmd_processchunk_cb")
	spawner:run()
end

function get_tns_name()
	return ls_file(props['FileDir'], "*.tns")
end

local tns = get_tns_name()
if tns == nil then
	dofile(props['SciteDefaultHome'] .. "/cmd_tools/build.lua")
	for i = 0, 5 do
		sleep(1)
		tns = get_tns_name()
		if tns ~= nil then break end
	end
end
if tns ~= nil then
	print("Transferring '" .. tns .. "'...")
	navnetcmd(tns)
end
