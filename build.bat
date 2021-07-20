@echo off
call clean.bat

echo.
echo Building OrigoToC...
if exist origotoc.c .\tools\tcc\tcc.exe origotoc.c

:CheckNext
if not "%1"=="retg1" goto SkipRetG1
if exist origotoc.exe echo.
if exist origotoc.exe echo Transpiling Retineo G1...
if exist origotoc.exe origotoc.exe retg1.ret

if exist retg1.rtc if exist retg1.rth echo.
if exist retg1.rtc if exist retg1.rth echo Building Retineo G1...
if exist retg1.rtc if exist retg1.rth .\tools\tcc\tcc.exe retg1.rtc

if exist retg1.exe echo.
if exist retg1.exe echo Building Retineo
if exist retg1.exe retg1 retineo.ret
:SkipRetG1

if not "%1"=="test" goto SkipTest
if exist origotoc.exe echo.
if exist origotoc.exe echo Transpiling Test...
if exist origotoc.exe origotoc test

if exist test.rtc if exist test.rth echo.
if exist test.rtc if exist test.rth echo Building Test...
if exist test.rtc if exist test.rth .\tools\tcc\tcc.exe -xc test.rtc
:SkipTest

if "%1"=="" goto SkipNext
shift
goto CheckNext
:SkipNext
