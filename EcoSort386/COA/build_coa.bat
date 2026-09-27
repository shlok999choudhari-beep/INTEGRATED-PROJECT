@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

echo ====================================================
echo  Building EcoSort 386 COA (64-Bit Architecture)
echo ====================================================

where nasm >nul 2>nul
if %ERRORLEVEL% EQU 0 (
    echo [FOUND] NASM detected. Assembling native x86-64 ecosort.asm...
    nasm -f win64 ecosort.asm -o ecosort.obj
    gcc ecosort.obj -o ecosort_coa.exe
) else (
    echo [INFO] NASM not in PATH. Compiling 64-bit architecture driver via Clang++...
    clang++ -O2 ecosort_coa_driver.cpp -o ecosort_coa.exe
)

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build complete: ecosort_coa.exe
    if "%~1"=="--run" (
        echo Running EcoSort 386 COA...
        ecosort_coa.exe
    )
) else (
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%.
)
endlocal
