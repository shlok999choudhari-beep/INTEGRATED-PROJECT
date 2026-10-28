@echo off
setlocal
cd /d "%~dp0"
if exist "C:\msys64\ucrt64\bin\g++.exe" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
if exist "C:\msys64\usr\bin\nasm.exe" set "PATH=C:\msys64\usr\bin;%PATH%"
where nasm >nul 2>nul || (echo [ERROR] NASM not found. Add NASM to PATH.& exit /b 1)
where g++ >nul 2>nul || (echo [ERROR] g++ not found. Use MSYS2 UCRT64.& exit /b 1)
nasm -f win64 control_functions.asm -o control_functions.obj
if errorlevel 1 exit /b 1
g++ -std=c++17 -O2 -static main.cpp control_functions.obj -o coa_co4_integration.exe
if errorlevel 1 exit /b 1
echo [SUCCESS] COA CO4 integration module built.
endlocal
