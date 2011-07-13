__oc = {} -- protected scope for all global variables, not to conflict with the programs run

toolpalette.register({})
toolpalette.enablePaste(true)

__oc.NEVER_RUN_CODE = true

function __oc.copy_table(t)
	local t2 = {}
	for k,v in pairs(t) do
		t2[k] = v
	end
	return t2
end

function __oc.trim(s)
	return (string.gsub(s, "^%s*(.-)%s*$", "%1"))
end

function __oc.loadcode(code)
	__oc.ERR = nil
	local chunk, errload = loadstring(code)
	if not chunk then
		__oc.ERR = errload
		platform.window:invalidate()
	else
		on = {} -- to find out which one are redefined
		local status, errcall = pcall(chunk)
		if not status then
			__oc.ERR = errcall
		else
			__oc.NEVER_RUN_CODE = false
		end
		platform.window:invalidate()
		__oc.reload_handlers()
	end
end

function __oc.reload_handlers()
	__oc.on = __oc.copy_table(on) -- those defined by the program

	on.paste = function() -- cannot be redefined
		if clipboard.getText() then
			__oc.loadcode(clipboard.getText())
		end
	end
	
	on.paint = function(gc)
		if __oc.ERR then
			gc:setFont("sansserif", "r", 8)
			local maxchars, y = 60, 10
			for i = 1, #(__oc.ERR), maxchars do
				gc:drawString(__oc.trim(string.sub(__oc.ERR, i, i + maxchars - 1)), 0, y)
				y = y + 10
			end
		else
			if __oc.on.paint then
				__oc.on.paint(gc)
			end
		end
		if __oc.ERR or __oc.NEVER_RUN_CODE then
			gc:setFont("sansserif", "r", 8)
			gc:drawString("Paste some Lua code here to run it", 85, 210)
		end
	end

end

__oc.reload_handlers()
