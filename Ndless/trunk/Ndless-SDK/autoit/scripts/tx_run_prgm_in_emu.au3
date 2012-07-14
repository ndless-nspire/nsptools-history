; Params: prgm_path

#include <GDIPlus.au3>
#include <Clipboard.au3>
#include <ScreenCapture.au3>
#include <WinAPI.au3>

#include <clipboard.au3>
#include <gdiplus.au3>
#include <guiconstantsex.au3>
#include <windowsconstants.au3>

If $CmdLine[0] <> 1 Then
	Exit 1
 EndIf
If WinWait("nspire_emu", "", 1) == 0 Then
   Exit 1
EndIf

WinActivate("nspire_emu")
Opt("SendKeyDownDelay", 100) ; else nspire_emu may not react
GoHome()

; Goes to the home screen, takes a screenshot, extracts a sample and checks that it matches the emu_home.png sample to make sure we are on the home screen.
Func GoHome()
   Send("{ESC}{ESC}{ESC}{HOME}")

   Send("{F7}") ; screenshot

   If Not _ClipBoard_Open(0) Then Exit _WinAPI_ShowError("_ClipBoard_Open failed")
   local $hMemory = _ClipBoard_GetDataEx($CF_BITMAP)
   If $hMemory = 0 Then Exit _WinAPI_ShowError("_ClipBoard_GetDataEx failed")

   _GDIPlus_Startup()
   local $hBitmap = _GDIPlus_BitmapCreateFromHBITMAP($hMemory)
   If $hBitmap = 0 Then Exit _WinAPI_ShowError("_GDIPlus_BitmapCreateFromHBITMAP failed")

   local $home_sample = _GDIPlus_ImageLoadFromFile(@ScriptDir & "\emu_home.png")
   If $home_sample = -1 or $home_sample = -0 Then Exit _WinAPI_ShowError("Home sample not found")

   local $hBitmap_extract = _GDIPlus_BitmapCloneArea($hBitmap, 0, 0, _GDIPlus_ImageGetWidth($home_sample), _GDIPlus_ImageGetHeight($home_sample))
   If $hBitmap_extract = -1 Then Exit _WinAPI_ShowError("_GDIPlus_BitmapCloneArea failed")

   If not CompareBitmaps($home_sample, $hBitmap_extract) Then Exit _WinAPI_ShowError("Can't reach the TI-Nspire home screen.")
   _GDIPlus_Shutdown()
EndFunc

; Returns true if the same
Func CompareBitmaps($bm1, $bm2)
    
    $Bm1W = _GDIPlus_ImageGetWidth($bm1)
    $Bm1H = _GDIPlus_ImageGetHeight($bm1)
    $BitmapData1 = _GDIPlus_BitmapLockBits($bm1, 0, 0, $Bm1W, $Bm1H, $GDIP_ILMREAD, $GDIP_PXF32RGB)
    $Stride = DllStructGetData($BitmapData1, "Stride")
    $Scan0 = DllStructGetData($BitmapData1, "Scan0")
    
    $ptr1 = $Scan0
    $size1 = ($Bm1H - 1) * $Stride + ($Bm1W - 1) * 4
    
    
    $Bm2W = _GDIPlus_ImageGetWidth($bm2)
    $Bm2H = _GDIPlus_ImageGetHeight($bm2)
    $BitmapData2 = _GDIPlus_BitmapLockBits($bm2, 0, 0, $Bm2W, $Bm2H, $GDIP_ILMREAD, $GDIP_PXF32RGB)
    $Stride = DllStructGetData($BitmapData2, "Stride")
    $Scan0 = DllStructGetData($BitmapData2, "Scan0")
    
    $ptr2 = $Scan0
    $size2 = ($Bm2H - 1) * $Stride + ($Bm2W - 1) * 4
    
    $smallest = $size1
    If $size2 < $smallest Then $smallest = $size2
    $call = DllCall("msvcrt.dll", "int:cdecl", "memcmp", "ptr", $ptr1, "ptr", $ptr2, "int", $smallest)
    
    _GDIPlus_BitmapUnlockBits($bm1, $BitmapData1)
    _GDIPlus_BitmapUnlockBits($bm2, $BitmapData2)
    
    Return ($call[0]=0)
    
EndFunc  ;==>CompareBitmaps


;local $winpos = WinGetPos("nspire_emu")
;local $trect = _WinAPI_GetClientRect(_WinAPI_FindWindow("nspire_emu", "nspire_emu"))
;local $client_x = DllStructGetData($tRect, "Left") 
;local $client_y = DllStructGetData($tRect, "Right") 
;local $csize = WinGetClientSize("nspire_emu")
;local $scrpos = ControlGetPos("nspire_emu", "", "[CLASS:nspire_gfx; INSTANCE:1]")
;MsgBox(4096, "", $scrpos[1])
;local $hBMP = _ScreenCapture_Capture("C:\temp\test.bmp", $client_x, $client_y, DllStructGetData($tRect, "Top"), DllStructGetData($tRect, "Bottom")  )
;local $hImage = _GDIPlus_BitmapCreateFromHBITMAP($hBMP)

;_ClipBoard_Open(0)
;local $sshot = _ClipBoard_GetData($CF_DIB)
;MsgBox(4096, "", _GDIPlus_BitmapCreateHBITMAPFromBitmap($sshot))

;MsgBox(4096, "", _ClipBoard_EnumFormats(2))

