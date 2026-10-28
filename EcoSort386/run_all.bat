@echo off
setlocal
cd /d "%~dp0"
rem COA and the graphical modules use the MSYS2 UCRT64 runtime DLLs.
if exist "C:\msys64\ucrt64\bin" set "PATH=C:\msys64\ucrt64\bin;%PATH%"
echo ====================================================
echo  EcoSort 386 - Integrated Multi-Subject System
echo ====================================================
echo Launching COA, PL/PSOOP, CGL CO1 and the master interface.
echo Close each window separately when finished.

rem Start the built programs directly.  This avoids nested cmd/call quoting
rem problems that can prevent the module batch files from being found.
start "EcoSort 386 - COA" /D "%~dp0COA" cmd.exe /k .\ecosort_coa.exe
start "EcoSort 386 - PL and PSOOP" /D "%~dp0PL and PSOOP" cmd.exe /k .\ecosort_pl_psoop.exe
start "EcoSort 386 - CGL CO1" /D "%~dp0CGL\CO1" .\ecosort_co1.exe
start "EcoSort 386 - Master Interface" /D "%~dp0Interface" .\ecosort_interface.exe
endlocal
