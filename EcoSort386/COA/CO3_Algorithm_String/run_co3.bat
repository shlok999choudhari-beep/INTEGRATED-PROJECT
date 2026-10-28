@echo off
setlocal
cd /d "%~dp0"
if not exist coa_co3_algorithm_string.exe (
    call build_co3_algorithm_string.bat
)
start "EcoSort 386 - COA CO3 Engine" coa_co3_algorithm_string.exe
endlocal
