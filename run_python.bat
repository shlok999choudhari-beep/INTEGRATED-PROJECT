@echo off
title Antigravity OpenGL Python Demo
echo ====================================================
echo  Launching Antigravity OpenGL Python 3D Demo...
echo ====================================================
python python\main.py
if %ERRORLEVEL% NEQ 0 (
    echo.
    echo [ERROR] Demo exited with error code %ERRORLEVEL%.
    pause
)
