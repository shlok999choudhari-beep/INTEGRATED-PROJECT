@echo off
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ====================================================
echo  Antigravity OpenGL - C++ Build & Run
echo ====================================================

set "PATH=C:\Program Files\CMake\bin;C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"

:: Configure build folder
if not exist build (
    echo [1/3] Configuring CMake project...
    cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
    if %ERRORLEVEL% NEQ 0 (
        echo [ERROR] CMake configuration failed.
        exit /b 1
    )
)

:: Compile
echo [2/3] Building target opengl_demo...
cmake --build build --config Release
if %ERRORLEVEL% NEQ 0 (
    echo [ERROR] Build failed.
    exit /b 1
)

:: Run
echo [3/3] Launching opengl_demo...
if exist build\Release\opengl_demo.exe (
    build\Release\opengl_demo.exe
) else if exist build\opengl_demo.exe (
    build\opengl_demo.exe
) else (
    echo [ERROR] Executable opengl_demo.exe not found.
)

endlocal
