@echo off
setlocal
cd /d "%~dp0"
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin\nasm.exe" set "PATH=C:\msys64\usr\bin;%PATH%"
where nasm >nul 2>nul || (echo [ERROR] NASM not found. Add NASM to PATH.& exit /b 1)
where g++ >nul 2>nul || (echo [ERROR] g++ not found. Use MSYS2 UCRT64.& exit /b 1)
nasm -f win64 algorithm_string_functions.asm -o algorithm_string_functions.obj
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -static main.cpp algorithm_string_functions.obj -o coa_co3_algorithm_string.exe
if errorlevel 1 exit /b 1
echo [SUCCESS] COA CO3 arithmetic and string module built.
endlocal
