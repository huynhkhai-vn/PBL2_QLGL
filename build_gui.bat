@echo off
setlocal

REM Raylib must be installed in the MSYS2 UCRT64 environment.
REM Run this script from the project root using the UCRT64 terminal or VS Code terminal.

g++ -std=c++17 -Wall -Wextra -Wpedantic -I. gui_main.cpp Algorithms/dijkstra/dijkstra.cpp DataStructures/intersection/intersection.cpp DataStructures/map/adjnode.cpp DataStructures/map/linkedlist.cpp DataStructures/map/map.cpp DataStructures/priorityqueue/hnode.cpp DataStructures/priorityqueue/minheap.cpp DataStructures/road/road.cpp -o pbl2_gui.exe -lraylib -lopengl32 -lgdi32 -lwinmm
if errorlevel 1 (
    echo.
    echo Build failed. Check that Raylib is installed and g++ is from MSYS2 UCRT64.
    pause
    exit /b 1
)

echo Build successful. Starting graphical interface...
pbl2_gui.exe
endlocal
