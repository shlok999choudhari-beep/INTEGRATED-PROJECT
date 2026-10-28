@echo off
setlocal
cd /d "%~dp0"

if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"

echo ====================================================
echo  EcoSort 386 - Integrated Multi-Subject System
echo ====================================================
echo.
echo Select an option to launch:
echo  [1] Launch Core Multi-Process Suite (COA, PL/PSOOP, CO1 Kiosk, Interface)
echo  [2] Launch CGL CO2: Automated Dynamic Sorting Assembly Line (Bresenham & DDA)
echo  [3] Launch CGL CO3: 2D/3D Transforms, Clipping & Dual Viewports
echo  [4] Launch CGL CO6: 3D Textured & Lit Truck Unloading Station
echo  [5] Launch COA CO3: Algorithmic Add-and-Shift & 64-Bit Assembly String Engine
echo  [6] Launch All Modules Simultaneously (Full Synchronized Suite)
echo.

set /p "CHOICE=Enter choice (1-6, default 6): "
if "%CHOICE%"=="" set CHOICE=6

if "%CHOICE%"=="1" goto launch_core
if "%CHOICE%"=="2" goto launch_co2
if "%CHOICE%"=="3" goto launch_co3
if "%CHOICE%"=="4" goto launch_co6
if "%CHOICE%"=="5" goto launch_coaco3
if "%CHOICE%"=="6" goto launch_all

:launch_core
echo Launching Core Multi-Process Suite...
start "EcoSort 386 - COA" /D "%~dp0COA" cmd.exe /k .\ecosort_coa.exe
start "EcoSort 386 - PL and PSOOP" /D "%~dp0PL and PSOOP" cmd.exe /k .\ecosort_pl_psoop.exe
start "EcoSort 386 - CGL CO1 Kiosk" /D "%~dp0CGL\CO1" .\ecosort_co1.exe
start "EcoSort 386 - Master Interface" /D "%~dp0Interface" .\ecosort_interface.exe
goto end

:launch_co2
echo Launching CGL CO2 Dynamic Sorting Line...
start "EcoSort 386 - CGL CO2 Conveyor Line" /D "%~dp0CGL\CO2" .\ecosort_co2.exe
goto end

:launch_co3
echo Launching CGL CO3 Transforms & Clipping...
start "EcoSort 386 - CGL CO3 Transformations & Clipping" /D "%~dp0CGL\CO3" .\ecosort_co3.exe
goto end

:launch_co6
echo Launching CGL CO6 3D Unloading Station...
start "EcoSort 386 - CGL CO6 3D Unloading Station" /D "%~dp0CGL\CO6" .\ecosort_co6.exe
goto end

:launch_coaco3
echo Launching COA CO3 Algorithmic & String Engine...
start "EcoSort 386 - COA CO3 Engine" /D "%~dp0COA\CO3_Algorithm_String" .\coa_co3_algorithm_string.exe
goto end

:launch_all
echo Launching All Synchronized Modules...
start "EcoSort 386 - COA" /D "%~dp0COA" cmd.exe /k .\ecosort_coa.exe
start "EcoSort 386 - COA CO3 Engine" /D "%~dp0COA\CO3_Algorithm_String" .\coa_co3_algorithm_string.exe
start "EcoSort 386 - PL and PSOOP" /D "%~dp0PL and PSOOP" cmd.exe /k .\ecosort_pl_psoop.exe
start "EcoSort 386 - CGL CO1 Kiosk" /D "%~dp0CGL\CO1" .\ecosort_co1.exe
start "EcoSort 386 - CGL CO2 Conveyor Line" /D "%~dp0CGL\CO2" .\ecosort_co2.exe
start "EcoSort 386 - CGL CO6 3D Unloading Station" /D "%~dp0CGL\CO6" .\ecosort_co6.exe
start "EcoSort 386 - Master Interface" /D "%~dp0Interface" .\ecosort_interface.exe
goto end

:end
echo [SUCCESS] Windows launched successfully.
endlocal
