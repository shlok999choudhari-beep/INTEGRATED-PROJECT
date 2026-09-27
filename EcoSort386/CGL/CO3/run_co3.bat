@echo off
setlocal
cd /d "%~dp0"
if not exist ecosort_co3.exe (
    call build_co3.bat
)
if exist ecosort_co3.exe (
    start "" ecosort_co3.exe
)
endlocal
