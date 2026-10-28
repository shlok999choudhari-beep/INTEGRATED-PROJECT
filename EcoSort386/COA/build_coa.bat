@echo off
setlocal
cd /d "%~dp0"

rem Add NASM and MinGW paths if present
if exist "C:\Users\shlok\AppData\Local\bin\NASM\nasm.exe" set "PATH=C:\Users\shlok\AppData\Local\bin\NASM;%PATH%"
if exist "C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin" set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\ucrt64\bin\gcc.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\ucrt64\bin\nasm.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin\nasm.exe" set "PATH=C:\msys64\usr\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 COA - 64-bit Assembly Integration
echo ====================================================

where nasm >nul 2>nul
if errorlevel 1 (
    echo [ERROR] NASM was not found. Install NASM and add it to PATH.
    exit /b 1
)
where g++ >nul 2>nul
if errorlevel 1 (
    echo [ERROR] g++ was not found. Open MSYS2 UCRT64 or add its bin folder to PATH.
    exit /b 1
)

nasm -f win64 ecosort_functions.asm -o ecosort_functions.obj
if errorlevel 1 exit /b 1

g++ -std=c++17 -O2 -static ecosort_coa_driver.cpp ecosort_functions.obj -o ecosort_coa.exe
if errorlevel 1 (
    echo [ERROR] COA driver build failed.
    exit /b 1
)
echo [SUCCESS] Built ecosort_coa.exe with actual NASM functions linked.

rem Also assemble and build standalone ecosort.exe from ecosort.asm
nasm -f win64 ecosort.asm -o ecosort.obj
if %ERRORLEVEL% EQU 0 (
    gcc -static ecosort.obj -o ecosort.exe
    if %ERRORLEVEL% EQU 0 (
        echo [SUCCESS] Built standalone assembly executable: ecosort.exe
    )
)

if /I "%~1"=="--run" ecosort_coa.exe
endlocal
