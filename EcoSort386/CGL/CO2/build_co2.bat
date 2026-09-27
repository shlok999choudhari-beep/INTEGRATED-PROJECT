@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 CGL CO2 (Bresenham and DDA)
echo ====================================================

clang++ -O2 main.cpp -I ../../../include -L ../../../build/_deps/glfw-build/src -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -static -o ecosort_co2.exe

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build complete: ecosort_co2.exe
    if "%~1"=="--capture-all" (
        echo Generating evidence screenshots...
        ecosort_co2.exe --capture-all
    ) else if "%~1"=="--run" (
        echo Launching EcoSort 386 CO2...
        start "" ecosort_co2.exe
    )
) else (
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%.
)
endlocal
