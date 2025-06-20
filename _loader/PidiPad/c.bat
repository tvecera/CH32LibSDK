@echo off
rem Compilation...

call ..\..\_c1.bat pidipad
loaderbin\LoaderBin.exe Loader.bin ..\loader_PidiPad.S
