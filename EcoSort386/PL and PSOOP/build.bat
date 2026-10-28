@echo off
setlocal
cd /d "%~dp0"
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\ucrt64\bin\gcc.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin\nasm.exe" set "PATH=C:\msys64\usr\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 PL and PSOOP Modules
echo ====================================================

rem Target 1: Integrated PL and PSOOP with IPC, Hash Table, and Exception Handling
echo Compiling integrated pipeline...
g++ -std=c++17 -O2 "PL&PSOOPCO1&2.cpp" -static -o ecosort_pl_psoop.exe
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Failed to compile ecosort_pl_psoop.exe.
    exit /b %ERRORLEVEL%
)
echo [SUCCESS] Built integrated binary: ecosort_pl_psoop.exe

rem Target 2: Standalone PL and PSOOP without Hashing (EcoSort_without_PL_CO5.cpp)
if exist "EcoSort_without_PL_CO5.cpp" (
    echo Compiling standalone syllabus pipeline without PL CO5...
    g++ -std=c++17 -O2 "EcoSort_without_PL_CO5.cpp" -static -o ecosort_without_pl_co5.exe
    if %ERRORLEVEL% EQU 0 (
        echo [SUCCESS] Built standalone binary: ecosort_without_pl_co5.exe
    ) else (
        echo [WARNING] Failed to compile ecosort_without_pl_co5.exe.
    )
)

if "%~1"=="--run" (
    echo Launching EcoSort 386 PL and PSOOP...
    ecosort_pl_psoop.exe
)
endlocal
