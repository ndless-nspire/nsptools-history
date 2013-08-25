@echo off
set prgm=navnetcmd.exe
if "%COMMONPROGRAMFILES(X86)%"=="" (set "common=%COMMONPROGRAMFILES%") else (set "common=%COMMONPROGRAMFILES(X86)%")
set PATH=%PATH%;%common%\TI Shared\CommLib\1\NavNet
%prgm% %*
