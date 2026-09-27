@echo off
setlocal
cd /d "%~dp0"
echo ====================================================
echo  Launching GLUT Application: EcoSort 386 - CGL CO1
echo ====================================================
start "" "%~dp0ecosort_triangle.exe"
endlocal
