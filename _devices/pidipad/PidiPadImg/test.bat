@echo off
md out

PidiPadImg in\imgdigits.bmp out\imgdigits.c ImgDigits 0
if errorlevel 1 goto err

PidiPadImg in\imgintro.bmp out\imgintro1.c ImgIntro1 1
if errorlevel 1 goto err

PidiPadImg in\imgintro.bmp out\imgintro2.c ImgIntro2 2
if errorlevel 1 goto err

PidiPadImg in\imgintro.bmp out\imgintro3.c ImgIntro3 3
if errorlevel 1 goto err

PidiPadImg in\imgintro.bmp out\imgintro4.c ImgIntro4 4
if errorlevel 1 goto err

PidiPadImg in\imgintro.bmp out\imgintro5.c ImgIntro5 5
if errorlevel 1 goto err

goto ok
:err
pause
:ok
