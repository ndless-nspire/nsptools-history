@echo off
set DIR=%~dp0
start %DIR%\logstash\run_agent.bat
%DIR%\nspire_emu.exe /1=C:\app\boot1.img.tns /MXC /F=C:\app\nand.img 1>%DIR%\nspire_emu.log 2>&1
