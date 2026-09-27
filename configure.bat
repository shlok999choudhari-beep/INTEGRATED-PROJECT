@echo off
set "PATH=C:\Program Files\CMake\bin;C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"
cmake -B build -S . -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
