@echo off
REM ##### Configure here #####
set NSPIRE_EMU_FLASH_IMAGE_PATH=flash_noncas_1.1.bin
REM CAS or NON_CAS
set HARDWARE=NON_CAS
REM ##########################

echo Creating strbackup.tns...
FlashEdit -b ../res/%HARDWARE%/strbackup.tns "%NSPIRE_EMU_FLASH_IMAGE_PATH%"
if %errorlevel% LEQ 0 goto end
echo.
echo Now:
echo - run the flash image with nspire_emu
echo - set the system language to English
echo - create a directory called 'ndless'
echo - send to this directory the files '../res/%HARDWARE%/strbackup.tns', '../res/%HARDWARE%/loader.tns' and '../res/%HARDWARE%/hook.tns'
echo - save the flash image
echo Then press enter.
pause
echo Installing the loader...
FlashEdit ../res/%HARDWARE%/loader.tns "%NSPIRE_EMU_FLASH_IMAGE_PATH%" %errorlevel%
echo.
echo Ndless is now installed in the flash image. You can restart nspire_emu.
:end
pause
