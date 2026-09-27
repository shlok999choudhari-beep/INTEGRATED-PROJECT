@echo off
setlocal
cd /d "%~dp0"
if not exist ecosort_pl_psoop.exe (
    call build.bat
)
if exist ecosort_pl_psoop.exe (
    ecosort_pl_psoop.exe
)
endlocal
