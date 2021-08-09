@echo off
call clean.bat

echo Generating OrigoToC parser from grammar...
.\tools\packcc\packcc origotoc.peg

if exist origotoc.c if exist origotoc.h echo Building OrigoToC parser...
if exist origotoc.c if exist origotoc.h tools\tcc\tcc origotoc.c

if exist origotoc.exe echo Adding version info to the OrigoToC executable...
if exist origotoc.exe .\tools\verpatch\verpatch origotoc.exe /va 0.0.1.1 /pv 0.0.1.1 /s copyright "(C) 2021 Orlando Llanes"
if not %errorlevel% == 0 if exist origotoc.exe del origotoc.exe

echo.
if exist origotoc.exe echo Done.
