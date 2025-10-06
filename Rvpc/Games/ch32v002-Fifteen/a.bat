@echo off
rem All Re-Compilation...

call d.bat
call c.bat %1
if errorlevel 1 goto stop
call e.bat
:stop
