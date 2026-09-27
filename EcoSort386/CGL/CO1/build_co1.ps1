param(
    [switch]$CaptureAll,
    [switch]$Run
)

Set-Location $PSScriptRoot

$env:PATH = "C:\Users\shlok\AppData\Local\Microsoft\WinGet\Packages\MartinStorsjo.LLVM-MinGW.UCRT_Microsoft.Winget.Source_8wekyb3d8bbwe\llvm-mingw-20260616-ucrt-x86_64\bin;" + $env:PATH

Write-Host "====================================================" -ForegroundColor Cyan
Write-Host " Building EcoSort 386 CGL CO1 (GLFW Style)" -ForegroundColor Cyan
Write-Host "====================================================" -ForegroundColor Cyan

clang++ -O2 main.cpp -I ../../../include -L ../../../build/_deps/glfw-build/src -lglfw3 -lopengl32 -lglu32 -lgdi32 -luser32 -lkernel32 -static -o ecosort_co1.exe

if ($LASTEXITCODE -eq 0) {
    Write-Host "[SUCCESS] Build complete: ecosort_co1.exe" -ForegroundColor Green
    if ($CaptureAll -or ($args -contains "--capture-all")) {
        Write-Host "Generating evidence screenshots..." -ForegroundColor Yellow
        .\ecosort_co1.exe --capture-all
    } elseif ($Run -or ($args -contains "--run")) {
        Write-Host "Launching EcoSort 386..." -ForegroundColor Yellow
        Start-Process ".\ecosort_co1.exe"
    }
} else {
    Write-Host "[ERROR] Compilation failed with error code $LASTEXITCODE." -ForegroundColor Red
}
