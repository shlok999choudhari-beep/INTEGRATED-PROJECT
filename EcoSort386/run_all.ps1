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
Start-Process -FilePath "cmd.exe" -ArgumentList "/k", "ecosort_coa.exe" -WorkingDirectory "$root\COA"

# 2. Launch PL & PSOOP in dedicated console
Start-Process -FilePath "cmd.exe" -ArgumentList "/k", "ecosort_pl_psoop.exe" -WorkingDirectory "$root\PL and PSOOP"

# 3. Launch CGL in its own window
Start-Process -FilePath "$root\CGL\CO1\ecosort_co1.exe" -WorkingDirectory "$root\CGL\CO1"

# 4. Launch Graphical Interface in its own window
Start-Process -FilePath "$root\Interface\ecosort_interface.exe" -WorkingDirectory "$root\Interface"

Write-Host "[SUCCESS] All 4 modules launched in synchronized windows!" -ForegroundColor Green
