; Outputs YES or NO
If WinWait("nspire_emu", "", 1) == 0 Then
   ConsoleWrite("NO")
   Exit 0
EndIf
ConsoleWrite("YES")