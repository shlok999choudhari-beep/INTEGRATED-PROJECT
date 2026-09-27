@echo off
set "PATH=C:\Program Files\CMake\bin;C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;%PATH%"
cmake -B freeglut-build/build -S freeglut-build/freeglut-3.4.0 -G "MinGW Makefiles" -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_BUILD_TYPE=Release -DFREEGLUT_BUILD_DEMOS=OFF
cmake --build freeglut-build/build --config Release
