@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 PL and PSOOP
echo ====================================================

clang++ -O2 "PL&PSOOPCO1&2.cpp" -static -o ecosort_pl_psoop.exe

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build complete: ecosort_pl_psoop.exe
    if "%~1"=="--run" (
        echo Launching EcoSort 386 PL and PSOOP...
        ecosort_pl_psoop.exe
    )
) else (
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%.
)
endlocal
