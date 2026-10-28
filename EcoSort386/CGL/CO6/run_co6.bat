@echo off
setlocal
cd /d "%~dp0"
if not exist ecosort_co6.exe (
    call build_co6.bat
)
start "EcoSort 386 - CGL CO6 3D Unloading Station" ecosort_co6.exe
endlocal
