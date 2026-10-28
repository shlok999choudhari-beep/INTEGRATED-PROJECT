@echo off
setlocal
cd /d "%~dp0"
if not exist "ecosort_without_pl_co5.exe" (
    echo ecosort_without_pl_co5.exe not found. Building now...
    call build.bat
)
echo Launching EcoSort 386 - Standalone PL & PSOOP (without PL CO5)...
ecosort_without_pl_co5.exe
endlocal
