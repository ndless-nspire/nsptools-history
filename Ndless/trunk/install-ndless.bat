@echo off
if "%OS%"=="Windows_NT" setlocal
if "%PROCESSOR_ARCHITECTURE%"=="AMD64" (
	set regkey=HKEY_LOCAL_MACHINE\SOFTWARE\Wow6432Node\Texas Instruments\TI-Nspire Computer Link
) else (
	set regkey=HKEY_LOCAL_MACHINE\SOFTWARE\Texas Instruments\TI-Nspire Computer Link
)

REM first format tested for parsing: with header, with <...> on the line searched
for /f "tokens=2 delims=>" %%a in ('reg query "%regkey%" /ve 2^>^&1') do set regline="%%a"
if ~%regline%~ == ~~ goto test_2nd_format
if ~%regline%~ == "~~" goto no_link
REM extract the path value
for /f "tokens=1* delims=Z " %%a in ('echo %regline%') do set linkpath=%%b
REM remove leading tab and trailing \"
set linkpath=%linkpath:~1,-2%
goto path_found

:test_2nd_format
REM second format tested for parsing: without header, with (...) on the line searched
for /f "tokens=1* delims=)" %%a in ('reg query "%regkey%" /ve 2^>^&1') do set regline="%%b"
if ~%regline%~ == "~~" goto no_link
REM extract the path value and remove leading spaces
for /f "tokens=2* delims=Z " %%a in ('echo %regline%') do set linkpath=%%b
REM remove trailing \"
set linkpath=%linkpath:~0,-2%

:path_found
set JAVA_HOME=%linkpath%
set JAVA_EXE=%JAVA_HOME%\jre\bin\javaw.exe

REM goto valid_JAVA_EXE
if exist "%JAVA_EXE%" goto valid_JAVA_EXE
:no_link
echo Error: TI-Nspire Computer Link doesn't seem to be installed on this computer
echo Get it at http://education.ti.com/educationportal/sites/US/productDetail/us_nspire_computerlink.html

:show_error
pause
goto end

:valid_JAVA_EXE
start /min "" "%JAVA_EXE%" "-jar" "res/install-ndless.jar"

:end
if "%OS%"=="Windows_NT" endlocal
