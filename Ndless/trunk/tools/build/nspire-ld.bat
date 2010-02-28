@echo off
set DIRNAME=%~dp0
set ARGS=

:argloop
if "%1"=="" goto argloopdone
	if "%1"=="-nostdlib" set nostdlib=true
	set ARGS=%ARGS% %1
shift
goto argloop
:argloopdone

if not "%nostdlib%"=="true" set ARGS=%ARGS% "%DIRNAME%/../system/crt0.o" "%DIRNAME%/../system/syscalls.o"

arm-elf-gcc -T "%DIRNAME%/../system/ldscript" %ARGS%
