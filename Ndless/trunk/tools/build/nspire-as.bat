@echo off
set DIRNAME=%~dp0
arm-elf-as -mcpu=arm7tdmi -I "%DIRNAME%/../include" -D GNU_AS %*
