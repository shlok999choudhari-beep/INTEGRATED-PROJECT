Set-Location $PSScriptRoot
if (-not (Test-Path "ecosort_co2.exe")) {
    .\build_co2.ps1
}
Start-Process ".\ecosort_co2.exe"
