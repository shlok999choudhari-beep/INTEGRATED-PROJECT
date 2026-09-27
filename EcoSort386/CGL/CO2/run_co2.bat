@echo off
cd /d "%~dp0"
if not exist "ecosort_co2.exe" (
    call build_co2.bat
)
start "" ecosort_co2.exe
