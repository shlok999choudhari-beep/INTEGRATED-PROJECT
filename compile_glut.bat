@echo off
setlocal
cd /d "%~dp0"
set "PATH=C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

if "%~1"=="" (
    set "SRC=ecosort_triangle.cpp"
    set "OUT=ecosort_triangle.exe"
) else (
    set "SRC=%~1"
    set "OUT=%~dpn1.exe"
)

echo ====================================================
echo  Compiling GLUT C++ Program: %SRC%
echo ====================================================
clang++ -O2 "%SRC%" -I include -DFREEGLUT_STATIC -L freeglut-build/build/lib -lfreeglut_static -lopengl32 -lglu32 -lgdi32 -lwinmm -static -o "%OUT%"

if %ERRORLEVEL% EQU 0 (
    echo [SUCCESS] Build complete: %OUT%
    echo Launching program...
    start "" "%OUT%"
) else (
    echo.
    echo [ERROR] Compilation failed with error code %ERRORLEVEL%.
    pause
)
endlocal
