@echo off
setlocal
cd /d "%~dp0"
call build_coa.bat
if exist ecosort_coa.exe (
    ecosort_coa.exe
)
endlocal
