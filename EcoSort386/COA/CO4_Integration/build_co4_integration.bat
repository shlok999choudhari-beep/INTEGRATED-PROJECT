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
if exist "C:\msys64\mingw64\bin\g++.exe" set "PATH=C:\msys64\mingw64\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 - COA CO4 System Integration
echo ====================================================

where nasm >nul 2>nul || (echo [ERROR] NASM not found. Add NASM to PATH.& exit /b 1)
where g++ >nul 2>nul || (echo [ERROR] 64-bit g++ not found. Use MSYS2 UCRT64.& exit /b 1)

echo [1/4] Assembling CO4 Control Functions (control_functions.asm)...
nasm -f win64 control_functions.asm -o control_functions.obj
if errorlevel 1 exit /b 1

echo [2/4] Assembling CO2 Native Arithmetic Functions (ecosort_functions.asm)...
nasm -f win64 "..\ecosort_functions.asm" -o ecosort_functions.obj
if errorlevel 1 exit /b 1

echo [3/4] Assembling CO3 Algorithmic and String Functions (algorithm_string_functions.asm)...
nasm -f win64 "..\CO3_Algorithm_String\algorithm_string_functions.asm" -o algorithm_string_functions.obj
if errorlevel 1 exit /b 1

echo [4/4] Compiling and Linking CO4 Integration Engine (main.cpp)...
g++ -std=c++17 -O2 -static main.cpp control_functions.obj ecosort_functions.obj algorithm_string_functions.obj -o coa_co4_integration.exe
if errorlevel 1 (
    echo [ERROR] Linking failed for coa_co4_integration.exe
    exit /b 1
)

if not exist coa_co4_integration.exe (
    echo [ERROR] Executable coa_co4_integration.exe was not created.
    exit /b 1
)

echo [SUCCESS] Built coa_co4_integration.exe successfully!

if /I "%~1"=="--test" (
    echo Running automated test suite...
    coa_co4_integration.exe --test
) else if /I "%~1"=="--run" (
    start "EcoSort 386 - COA CO4 Engine" coa_co4_integration.exe
)

endlocal
