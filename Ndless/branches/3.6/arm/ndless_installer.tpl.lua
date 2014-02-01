-- The contents of this file are subject to the Mozilla Public
-- License Version 1.1 (the "License"); you may not use this file
-- except in compliance with the License. You may obtain a copy of
-- the License at http://www.mozilla.org/MPL/
-- 
--  The Original Code is Ndless code.
--
-- The Initial Developer of the Original Code is Jim Bauwens.

platform.apiLevel = "2.2"

function getVersion()
    local version

    -- is color?
    local color = platform.isColorDisplay()
    
    -- is cas?
    local _, caserr = math.eval("solve()")
    local has_cas = caserr == 930
    
    -- is computer?
	local hw
	if not platform.hw then
		hw = 0
	else
		hw = platform.hw()
	end
    local computer = hw == 7
    
    -- is tablet?
    local tablet = platform.isTabletModeRendering and platform.isTabletModeRendering()
    
    -- So what's the device
    local device
    
    if tablet then
        device = "TABLET"
    elseif computer then
        device = "COMPUTER"
    elseif color then
        device = "CX"
    else
        device = "BW"
    end
	
	local min_os
	if Widget then
		min_os = "3.6"
	else
		min_os = "0.0"
	end
	
    return device, has_cas, min_os
end 

-- The actual registration of the toolpalette must happen in on.resize when the Lua script is properly initialized
-- (otherwise we can't detect if it's cas/non-cas)
function on.resize()
    local device, has_cas, min_os = getVersion()
	if min_os == "3.6" then
		if device == "BW" and not has_cas then 
			s = s .. s_ncas
		elseif  device == "BW" and has_cas then 
			s = s .. s_cas
		elseif  device == "CX" and not has_cas then 
			s = s .. s_ncascx
		elseif device == "CX" and has_cas then
			s = s .. s_cascx
		end
		if not ndless then
			toolpalette.register{{s}}
		else
			-- TODO: confirmation popup
			ndless.uninst()
		end
	else
		-- TODO: error message for v3.1
	end

    -- avoid running again
    on.resize = nil
end

-- The "s" string building code goes here as produced by MakeLuaInst
