@echo off
setlocal
cd /d "%~dp0"

if not exist "coa_co4_integration.exe" (
    echo [INFO] Binary not found. Building COA CO4 Integration Module...
    call build_co4_integration.bat
    if errorlevel 1 (
        echo [ERROR] Build failed. Cannot run COA CO4.
        pause
        exit /b 1
    )
)

echo Starting EcoSort 386 - COA CO4 System Integration...
.\coa_co4_integration.exe %*
endlocal
