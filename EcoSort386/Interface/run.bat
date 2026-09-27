@echo off
setlocal
cd /d "%~dp0"
if not exist ecosort_interface.exe (
    call build.bat
)
if exist ecosort_interface.exe (
    ecosort_interface.exe
)
endlocal
