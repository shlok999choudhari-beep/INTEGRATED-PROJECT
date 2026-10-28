@echo off
setlocal
cd /d "%~dp0"

if exist "C:\Users\shlok\AppData\Local\bin\NASM\nasm.exe" set "PATH=C:\Users\shlok\AppData\Local\bin\NASM;%PATH%"
if exist "C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin" set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin\nasm.exe" set "PATH=C:\msys64\usr\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 COA CO3: Algorithm and String
echo ====================================================

where nasm >nul 2>nul || (echo [ERROR] NASM not found. Add NASM to PATH.& exit /b 1)
where g++ >nul 2>nul || (echo [ERROR] g++ not found. Use MSYS2 UCRT64 or llvm-mingw.& exit /b 1)

nasm -f win64 algorithm_string_functions.asm -o algorithm_string_functions.obj
if errorlevel 1 exit /b 1

g++ -std=c++17 -O2 -static main.cpp algorithm_string_functions.obj -o coa_co3_algorithm_string.exe
if errorlevel 1 (
    echo [ERROR] Build failed.
    exit /b 1
)

echo [SUCCESS] Built coa_co3_algorithm_string.exe with 64-bit assembly linked.

if /I "%~1"=="--test" (
    echo Running automated test suite...
    coa_co3_algorithm_string.exe --test
) else if /I "%~1"=="--run" (
    start "EcoSort 386 - COA CO3 Engine" coa_co3_algorithm_string.exe
)
endlocal
