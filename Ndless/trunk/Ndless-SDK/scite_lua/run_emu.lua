-- TODO if already running, to launch it
-- auto save flash ?

local fn = package.loadlib(props['SciteDefaultHome'] .. "/spawner-ex/spawner-ex.dll", "luaopen_spawner")
if not fn then
	print("Can't load spawner-ex")
	return
end
fn()

-- resmask can contain any bash wildcard
-- return nil if not found
function getres(resmask, errmsg)
	local respath_prefix = props['SciteDefaultHome'] .. "/emu_resources"
	local out = spawner.popen(props['ndls.sh'] .. " 'ls \"" .. respath_prefix .. "/\"" .. resmask .. "'")
	for line in out:lines() do
		out = line
		break
	end
	if string.find(out, "No such file")	then
		if errmsg then
			print(errmsg)
			spawner.popen("explorer \"" .. props['SciteDefaultHome'] .. "\\emu_resources" .. "\"")
			print("The emulator must be set up, please refer to _ReadMe.txt.")
		end
		return nil
	else
		return out
	end
end

if scite_FileExists(props['SciteDefaultHome'] .. "/emu_resources/nand.img") then
	
	
else
	local txx = getres("*.tco")
	if not txx then txx = getres("*.tcc", "OS not found.") end
	if txx then
		local boot1 = getres("boot1.img.tns", "Boot1 not found.")
		if boot1 then
			local boot2 = getres("boot2.img.tns", "Boot2 not found.")
			if boot2 then
				local casswitch = string.find(txx, ".tcc$") and "C" or ""
				local ns_spawner = spawner.new(props['SciteDefaultHome'] .. "\\nspire_emu\\nspire_emu.bat \"/1=" .. boot1 .. "\" \"/PO=" .. txx .. "\" \"/PB=" .. boot2 .. "\" /MX" .. casswitch)
				ns_spawner:run()
			end
		end
	end
 
end
