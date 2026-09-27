# EcoSort 386 - Master Synchronized Launcher (PowerShell)
$root = $PSScriptRoot

Write-Host "====================================================" -ForegroundColor Cyan
Write-Host " Launching EcoSort 386 Integrated Multi-Subject System" -ForegroundColor Green
Write-Host "====================================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "Starting 4 synchronized modules..." -ForegroundColor Yellow
Write-Host "1. COA: 64-Bit Processor Register Telemetry"
Write-Host "2. PL & PSOOP: Interactive Queue & Segregation CLI"
Write-Host "3. CGL: Smart City 2D Kiosk & Animated Collection Truck"
Write-Host "4. Interface: Master Graphical Command Panel (Working Buttons)"
Write-Host ""

# 1. Launch COA in dedicated console
Start-Process cmd.exe -ArgumentList "/k cd /d `"$root\COA`" && call run_coa.bat"

# 2. Launch PL & PSOOP in dedicated console
Start-Process cmd.exe -ArgumentList "/k cd /d `"$root\PL and PSOOP`" && call run.bat"

# 3. Launch CGL in its own window
Start-Process cmd.exe -ArgumentList "/c cd /d `"$root\CGL\CO1`" && call run_co1.bat"

# 4. Launch Graphical Interface in its own window
Start-Process cmd.exe -ArgumentList "/c cd /d `"$root\Interface`" && call run.bat"

Write-Host "[SUCCESS] All 4 modules launched in synchronized windows!" -ForegroundColor Green
