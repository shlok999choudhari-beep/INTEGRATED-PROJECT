@echo off
setlocal
cd /d "%~dp0"

echo ====================================================
echo  Launching EcoSort 386 Integrated Multi-Subject System
echo ====================================================
echo.
echo Starting 3 synchronized modules...
echo 1. COA: 64-Bit Processor Register Telemetry
echo 2. PL and PSOOP: Interactive Queue and Segregation CLI
echo 3. CGL: Smart City 2D Kiosk and Animated Collection Truck
echo.

:: 1. Launch COA in its own console window
start "EcoSort 386 - COA 64-Bit Register Telemetry" cmd /k "cd /d "%~dp0COA" && call run_coa.bat"

:: 2. Launch PL & PSOOP in its own interactive console window
start "EcoSort 386 - PL and PSOOP Queue & Segregation" cmd /k "cd /d "%~dp0PL and PSOOP" && call run.bat"

:: 3. Launch CGL CO1 Interactive Kiosk & Animated Moving Truck
start "EcoSort 386 - CGL Graphics Kiosk" cmd /c "cd /d "%~dp0CGL\CO1" && call run_co1.bat"

echo [SUCCESS] All 3 modules launched in synchronized windows!
endlocal
