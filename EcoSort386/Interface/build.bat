@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 Master Control Interface
echo ====================================================

clang++ -O2 main.cpp -I ../../include -L ../../build/_deps/glfw-build/src -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -static -o ecosort_interface.exe

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build complete: ecosort_interface.exe
    if "%~1"=="--run" (
        echo Launching EcoSort 386 Control Interface...
        ecosort_interface.exe
    )
) else (
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%.
)
endlocal
