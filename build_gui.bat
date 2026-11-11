@echo off
setlocal

cd /d "%~dp0"

where g++ >nul 2>nul
if errorlevel 1 (
    echo Build failed: g++ was not found. Use the MSYS2 UCRT64 environment.
    exit /b 1
)

where pkg-config >nul 2>nul
if errorlevel 1 (
    echo Build failed: pkg-config was not found. Use the MSYS2 UCRT64 environment.
    exit /b 1
)

pkg-config --exists Qt6Widgets
if errorlevel 1 (
    echo Build failed: Qt 6 Widgets was not found.
    echo Install it with: pacman -S mingw-w64-ucrt-x86_64-qt6-base
    exit /b 1
)

for /f "delims=" %%i in ('pkg-config --cflags Qt6Widgets') do set "QT_CFLAGS=%%i"
for /f "delims=" %%i in ('pkg-config --libs Qt6Widgets') do set "QT_LIBS=%%i"

g++ -std=c++17 -Wall -Wextra -Wpedantic -mwindows -Iinclude ^
    gui_main.cpp -o pbl2_gui.exe %QT_CFLAGS% %QT_LIBS%
if errorlevel 1 (
    echo.
    echo Build failed. Check that Qt 6 Widgets is installed in MSYS2 UCRT64.
    exit /b 1
)

echo Build successful. Starting graphical interface...
pbl2_gui.exe
exit /b %errorlevel%
