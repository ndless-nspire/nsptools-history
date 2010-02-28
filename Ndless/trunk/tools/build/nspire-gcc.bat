@echo off
set DIRNAME=%~dp0
arm-elf-gcc -mcpu=arm7tdmi -I "%DIRNAME%/../include" %*
