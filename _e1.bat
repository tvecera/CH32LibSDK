@echo off
%~dp0_tools\make.exe flash
if not errorlevel 1 goto end
pause
:end
