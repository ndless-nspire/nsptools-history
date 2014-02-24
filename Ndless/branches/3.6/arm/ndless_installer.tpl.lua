-- The contents of this file are subject to the Mozilla Public
-- License Version 1.1 (the "License"); you may not use this file
-- except in compliance with the License. You may obtain a copy of
-- the License at http://www.mozilla.org/MPL/
-- 
--  The Original Code is Ndless code.
--
-- The Initial Developer of the Original Code is Jim Bauwens.

-------------------
-- Set API Level --
-------------------

platform.apilevel = '1.0'

-----------------------------
-- Constants and variables --
-----------------------------

-- ndless_revision is set by ndless_version.lua
NDLESS_VERSION = "Ndless 3.6 rev. " .. ndless_revision

-- Will be corrected at runtime
DEVICE_MODEL   = ""
DEVICE_HAS_CAS = false
DEVICE_MIN_OS  = ""

----------------------
-- Extend some libs --
----------------------

do

    function invalidateAll()
        Widget:setFocus(false)
        Widget:setFocus(true)
    end
    
    function AddToGC(key, func)
            local gcMetatable = getmetatable(platform.gc())
            gcMetatable[key] = func
    end
    
    local function drawBWHorizontalGradient(gc, from, to, x, y, w, h)
        local diff = to - from
        for i=1, h do
            local shade = math.floor((i * diff) / h)
            gc:setColorRGB(shade, shade, shade)
            gc:fillRect(x, y + i - 1, w, 1)
        end 
    end
    
    local function drawStringMiddle(gc, str, x, y, w, h)
        local sw = gc:getStringWidth(str)
        local sh = gc:getStringHeight(str)
        
        
        gc:drawString(str, x + (w - sw) / 2, y + (h - sh) / 2, "top")
    end
    
    AddToGC("drawBWHorizontalGradient", drawBWHorizontalGradient)
    AddToGC("drawStringMiddle", drawStringMiddle)

end

-----------------------
-- General functions --
-----------------------

local counter = 0
function refreshAtStart()
	counter = (counter + 1) % 100
	if counter == 5 then
		on.getFocus()
	end
end

function imgTest()
	image.new("\255\255\255\127\1\0\0\0\0\0\0\0\0\0\0\0\16\0\1\0")
end

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
		if pcall(imgTest) then
			min_os = "3.2"
		else
			min_os = "3.6"
		end
	else
		min_os = "0.0"
	end
	
    return device, has_cas, min_os
end 

---------------
-- Main view --
---------------

function drawMainView(gc)
    gc:drawBWHorizontalGradient(0, 80, 0, 1, 320, 21)
    gc:setColorRGB(50, 50, 50)
    gc:fillRect(0, 22, 320, 1)
    
    gc:setColorRGB(0xCC, 0xCC, 0xCC)
    gc:fillRect(0, 23, 320, 1)
    
    gc:setColorRGB(0, 0, 0)
    gc:fillRect(0, 24, 320, 240-24)
    
    gc:setColorRGB(0xDD, 0xDD, 0xFF)
    gc:setFont("sansserif", "r", 12)
    
    if not ndless then
        gc:drawStringMiddle("Install Ndless", 0, 0, 320, 24)
    else
        gc:drawStringMiddle("Uninstall Ndless", 0, 0, 320, 24)
    end
    
    gc:setColorRGB(0xDD, 0xFF, 0xDD)
    gc:setFont("sansserif", "r", 9)
    local b = string.uchar(10003)
    local bb = ":(" --string.uchar(215)
    
    if not (DEVICE_MIN_OS == "3.6") then
        gc:drawString(bb .. " This OS is not compatible with Ndless 3.6!", 5, 30, "top")
    elseif ndless then
        gc:drawString(b .. " Press [U] to uninstall Ndless", 5, 30, "top")
        gc:drawString(b .. " Device will reboot", 5, 44, "top")
        gc:drawString(b .. " To reinstall just reopen this document!", 5, 58, "top")  
    else
        gc:drawString(b .. " Press [MENU] to install Ndless", 5, 30, "top")
        gc:drawString(b .. " Installation will start immediately", 5, 44, "top")
        gc:drawString(b .. " Enjoy!", 5, 58, "top")  
    end

    gc:setColorRGB(0xCC, 0xCC, 0xCC)
    gc:fillRect(0, 82, 320, 1)
   
    gc:setColorRGB(0xDD, 0xFF, 0xDD)
    gc:setFont("sansserif", "r", 9)
    local b = string.uchar(61696)
    
    gc:drawString(b .. " " .. NDLESS_VERSION .. " ", 5, 86, "top")
    gc:drawString(b .. " Device: TI-Nspire " .. DEVICE_MODEL .. (DEVICE_HAS_CAS and " CAS" or ""), 5, 100, "top")
   
    gc:setColorRGB(0xCC, 0xCC, 0xCC)
    gc:fillRect(0, 122, 320, 1)
   
    gc:setColorRGB(0xFF, 0xDD, 0xDD)
    gc:drawStringMiddle("Developers & Testers", 5, 168, 320, 20)
end

------------------
-- Credits code --
------------------

local credits = {
    "ExtendeD",
    "bsl",
    "Goplat",
    "Lionel Debroux",
    "Levak",
    "Excale",
    "Vogtinator",
    "Critor",
    "Tangrs",
    "Jim Bauwens"
}

local PAUSE_SPOT = 0
local DELAY = 40
local STEP = 4
local START = -320
local END = 320

local cr = 1
local cp = START
local mc = 0

function drawCredits(gc)
    gc:setColorRGB(0, 0, 0)
    gc:fillRect(0,0,320,240)

    gc:setColorRGB(0xFF, 0xDD, 0xDD)
    gc:setFont("serif", "r", 14)
    gc:drawStringMiddle(credits[cr], cp, 170, 320, 20)    
end

---------------------
-- Draw everything --
---------------------

local is_invalidated = true

function on.paint(gc)
    if is_invalidated then
        is_invalidated = false
        
        gc:begin()
        drawMainView(gc)
        gc:finish()
    end
    
    drawCredits(gc)
end

function on.getFocus()
    is_invalidated = true
    platform.window:invalidate()
end

---------------------------------
-- Setup timer for credits bar --
---------------------------------

function on.create()
    timer.start(0.01)
end


function on.timer()
    if mc > 0 then
        mc = mc - 1
    elseif cp == -PAUSE_SPOT then
        cp = cp + STEP
        mc = DELAY
    elseif cp >= END then
        cp = START
        cr = cr + 1
        if cr > #credits then cr = 1 end
    else
        cp = cp + STEP
    end
    
    platform.window:invalidate(0,160,320,80)
	refreshAtStart()
end

----------------------
-- Ndless uninstall --
----------------------

function on.charIn(ch)
    if ch == "u" and ndless then
        ndless.uninst()
    end
end

----------------------
-- Ndless installer --
----------------------

function on.resize()
    local device, has_cas, min_os = getVersion()
    
    DEVICE_MODEL   = device
    DEVICE_HAS_CAS = has_cas
    DEVICE_MIN_OS  = min_os
    
	if min_os == "3.6" and s then
		if device == "BW" and not has_cas then 
			s = s .. s_ncas
		elseif  device == "BW" and has_cas then 
			s = s .. s_cas
		elseif  device == "CX" and not has_cas then 
			s = s .. s_ncascx
		elseif device == "CX" and has_cas then
			s = s .. s_cascx
		end
		
		if not ndless and s then
			toolpalette.register{{s}}
		end
	end

    -- avoid running again
    on.resize = nil
end

-- The "s" string building code goes here as produced by MakeLuaInst
