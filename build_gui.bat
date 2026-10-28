@echo off
setlocal

cd /d "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
    echo Build failed: g++ was not found. Use the MSYS2 UCRT64 environment.
    exit /b 1
)

g++ -std=c++17 -Wall -Wextra -Wpedantic -Iinclude ^
    gui_main.cpp ^
    -o pbl2_gui.exe ^
    -lraylib -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
    echo.
    echo Build failed. Check that Raylib is installed in MSYS2 UCRT64.
    exit /b 1
)

echo Build successful. Starting graphical interface...
pbl2_gui.exe
exit /b %errorlevel%
