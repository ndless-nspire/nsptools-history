scite.StripShow("!'Program name for the project:'[]((OK))(&Cancel)")

function silentexec(command)
	-- dirty hack
	props["command.49.*"] = command
	-- trigger user command #49
	scite.MenuCommand(1149)
	props["command.49.*"] = nil
end

function OnStrip(control, change)
	if control == 2 and change == 1  --[[ clicked --]] then
		scite.StripShow("") -- hide
		silentexec(props['SciteDefaultHome'] .. "/mingw-get/msys/1.0/bin/sh -l -c " .. "\"'" .. props['SciteDefaultHome'] .. "/ndless/bin/nspire-tools' new '" .. scite.StripValue(1) .. "'\"")
	end
end
