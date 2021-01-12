@echo off
if exist origotoc.exe del origotoc.exe
tcc origotoc.c -Wall
if exist origotoc.exe .\verpatch\verpatch origotoc.exe /va 0.0.1.1 /pv 0.0.1.1 /s copyright "2020 Orlando Llanes"
