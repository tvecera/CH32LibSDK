@echo off

rem Compilation... Compile one project
rem Command parameter %1 = target device
set DEVICE=%1

rem ---------------------------------------------------
rem Edit this line to setup path to GCC RISC-V compiler
rem ---------------------------------------------------
set GCC_CH32_PATH=C:\GCC_CH32\bin

rem Auto-update path
if "%GCC_CH32_PATH_ISSET%"=="YES" goto update_ok
set GCC_CH32_PATH_ISSET=YES
set PATH=%GCC_CH32_PATH%;%PATH%
:update_ok

%~dp0_tools\make.exe all
rem If you want to see all error messages, compile using this command:
rem %~dp0_tools\make.exe all 2> err.txt
if not errorlevel 1 goto end

rem Compilation error, stop and wait for key press
pause ERROR!

:end
