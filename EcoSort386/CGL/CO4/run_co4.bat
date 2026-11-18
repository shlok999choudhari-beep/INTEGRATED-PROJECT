@echo off
setlocal
cd /d "%~dp0"
if not exist ecosort_co4.exe (
    call build_co4.bat
)
start "EcoSort 386 - CGL CO4 Polygon Filling" ecosort_co4.exe
endlocal
