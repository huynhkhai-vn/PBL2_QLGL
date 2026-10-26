#include "raylib.h"
#include "DataStructures/map/map.h"
#include "Algorithms/dijkstra/dijkstra.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace {
constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;
const Color PBL_BG = { 15, 23, 42, 255 };
const Color PBL_PANEL = { 25, 36, 58, 255 };
const Color PBL_ROAD = { 75, 91, 116, 255 };
const Color PBL_BLUE = { 59, 130, 246, 255 };
const Color PBL_GREEN = { 34, 197, 94, 255 };
const Color PBL_RED = { 239, 68, 68, 255 };
const Color PBL_YELLOW = { 250, 204, 21, 255 };
const Color PBL_MUTED = { 164, 178, 199, 255 };

struct ScreenPoint { float x; float y; };

bool button(Rectangle r, const char* label, bool active = true) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, r);
    Color fill = active ? (hover ? Color{ 71, 85, 105, 255 } : Color{ 51, 65, 85, 255 })
                        : Color{ 45, 55, 72, 255 };
    DrawRectangleRounded(r, 0.18f, 8, fill);
    DrawRectangleRoundedLines(r, 0.18f, 8, active ? PBL_BLUE : Color{ 65, 75, 90, 255 });
    int tw = MeasureText(label, 16);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - 16) / 2), 16,
             active ? RAYWHITE : PBL_MUTED);
    return active && hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

std::string number(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}
}

int main() {
    map city;
    if (!city.load_i("Data/intersections.txt") || !city.load_r("Data/roads.txt")) {
        InitWindow(700, 180, "PBL2 - Loi du lieu du du lieu");
        BeginDrawing();
        ClearBackground(PBL_BG);
        DrawText("Khong doc duoc du lieu ban do!", 24, 30, 22, PBL_RED);
        DrawText("Hay chay chuong trinh tu thu muc goc PBL2 - Copy.", 24, 75, 18, RAYWHITE);
        EndDrawing();
        WaitTime(3.0);
        CloseWindow();
        return 1;
    }

    const int n = city.get_num_i();
    if (n < 2) return 1;
    std::vector<int> path(n), roadIds(n);
    int pathCount = 0;
    int startIndex = 0;
    int endIndex = n - 1;
    bool fastest = false;
    bool hasRoute = false;
    bool taxiRunning = false;
    double taxiStarted = 0.0;
    double routeDistance = 0.0;
    double routeTime = 0.0;
    std::string status = "Chon diem di/den, sau do bam Tim duong.";

    auto calculateRoute = [&]() {
        pathCount = 0;
        std::fill(path.begin(), path.end(), 0);
        std::fill(roadIds.begin(), roadIds.end(), 0);
        dijkstra algorithm;
        int startId = city.getIntersectionAt(startIndex).getid_i();
        int endId = city.getIntersectionAt(endIndex).getid_i();
        if (fastest) algorithm.fastestPath(city, startId, endId, path.data(), pathCount, roadIds.data());
        else algorithm.shortestPath(city, startId, endId, path.data(), pathCount, roadIds.data());
        hasRoute = pathCount > 0;
        taxiRunning = false;
        routeDistance = 0.0;
        routeTime = 0.0;
        if (hasRoute) {
            for (int i = 0; i + 1 < pathCount; ++i) {
                if (i < city.get_num_r()) {
                    routeDistance += std::max(0.0, city.getRoadDistanceById(roadIds[i], path[i], path[i + 1]));
                    routeTime += std::max(0.0, city.getRoadTimeById(roadIds[i], path[i], path[i + 1]));
                }
            }
            status = fastest ? "Da tim tuyen nhanh nhat." : "Da tim tuyen ngan nhat.";
        } else {
            status = "Khong tim thay duong di.";
        }
    };

    auto pointFor = [&](int id) -> ScreenPoint {
        const double minX = 10.0, maxX = 90.0, minY = 15.0, maxY = 100.0;
        const intersection* item = nullptr;
        for (int i = 0; i < city.get_num_i(); ++i) {
            if (city.getIntersectionAt(i).getid_i() == id) {
                item = &city.getIntersectionAt(i);
                break;
            }
        }
        if (!item) return { 0, 0 };
        return { 45.0f + (float)((item->getx() - minX) / (maxX - minX) * 650.0),
                 690.0f - (float)((item->gety() - minY) / (maxY - minY) * 560.0) };
    };

    InitWindow(SCREEN_W, SCREEN_H, "PBL2 | Ban do va tim duong Dijkstra");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(PBL_BG);
        DrawText("PBL2 - MO PHONG BAN DO GIAO THONG", 32, 22, 27, RAYWHITE);
        DrawText("Raylib + OOP C++ + thuat toan Dijkstra", 34, 55, 16, PBL_MUTED);
        DrawRectangleRounded({ 22, 88, 780, 686 }, 0.025f, 8, PBL_PANEL);
        DrawText("BAN DO GIAO LO", 42, 103, 17, PBL_MUTED);

        // Draw all roads first; selected route roads are highlighted.
        for (int i = 0; i < city.get_num_r(); ++i) {
            const road& r = city.getRoadAt(i);
            ScreenPoint a = pointFor(r.getstart_id());
            ScreenPoint b = pointFor(r.getend_id());
            bool onRoute = false;
            if (hasRoute) {
                for (int j = 0; j + 1 < pathCount; ++j) {
                    if (roadIds[j] == r.getid_r()) { onRoute = true; break; }
                }
            }
            DrawLineEx({ a.x, a.y }, { b.x, b.y }, onRoute ? 5.0f : 2.0f, onRoute ? PBL_YELLOW : PBL_ROAD);
            char label[32];
            std::snprintf(label, sizeof(label), "%.1f", r.getdistance());
            DrawText(label, (int)((a.x + b.x) / 2 + 3), (int)((a.y + b.y) / 2 - 14), 10,
                     onRoute ? PBL_YELLOW : PBL_MUTED);
        }

        // Taxi moves along the chosen path, interpolating between intersections.
        ScreenPoint taxiPoint{ 0, 0 };
        bool drawTaxi = false;
        if (taxiRunning && hasRoute && pathCount > 0) {
            double elapsed = GetTime() - taxiStarted;
            double duration = std::max(1.0, (double)(pathCount - 1) * 1.1);
            double progress = elapsed / duration;
            if (progress >= 1.0) {
                taxiRunning = false;
                status = "Taxi da den noi.";
            }
            double segmentPos = std::min(progress, 0.9999) * std::max(1, pathCount - 1);
            int segment = std::min((int)segmentPos, std::max(0, pathCount - 2));
            float t = (float)(segmentPos - segment);
            ScreenPoint a = pointFor(path[segment]);
            ScreenPoint b = pointFor(path[std::min(segment + 1, pathCount - 1)]);
            taxiPoint = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
            drawTaxi = true;
        }

        for (int i = 0; i < n; ++i) {
            const intersection& node = city.getIntersectionAt(i);
            ScreenPoint p = pointFor(node.getid_i());
            Color c = PBL_BLUE;
            if (node.getid_i() == city.getIntersectionAt(startIndex).getid_i()) c = PBL_GREEN;
            if (node.getid_i() == city.getIntersectionAt(endIndex).getid_i()) c = PBL_RED;
            if (hasRoute) {
                for (int j = 0; j < pathCount; ++j) {
                    if (path[j] == node.getid_i() && j > 0 && j < pathCount - 1) c = PBL_YELLOW;
                }
            }
            DrawCircle((int)p.x, (int)p.y, 12, c);
            DrawCircleLines((int)p.x, (int)p.y, 12, RAYWHITE);
            char idText[12];
            std::snprintf(idText, sizeof(idText), "%d", node.getid_i());
            DrawText(idText, (int)p.x - MeasureText(idText, 12) / 2, (int)p.y - 6, 12, RAYWHITE);
            std::string name = node.getname_i();
            if (name.size() > 20) name = name.substr(0, 18) + "..";
            DrawText(name.c_str(), (int)p.x + 14, (int)p.y - 8, 11, RAYWHITE);
        }
        if (drawTaxi) {
            DrawCircle((int)taxiPoint.x, (int)taxiPoint.y, 8, PBL_YELLOW);
            DrawCircleLines((int)taxiPoint.x, (int)taxiPoint.y, 8, BLACK);
        }

        // Right control panel
        DrawRectangleRounded({ 820, 88, 440, 686 }, 0.025f, 8, PBL_PANEL);
        if (button({ 850, 145, 42, 34 }, "<")) { startIndex = (startIndex - 1 + n) % n; hasRoute = false; }
        if (button({ 1220, 145, 42, 34 }, ">")) { startIndex = (startIndex + 1) % n; hasRoute = false; }
        if (button({ 850, 205, 42, 34 }, "<")) { endIndex = (endIndex - 1 + n) % n; hasRoute = false; }
        if (button({ 1220, 205, 42, 34 }, ">")) { endIndex = (endIndex + 1) % n; hasRoute = false; }
        if (button({ 850, 270, 188, 42 }, "Duong ngan nhat", !taxiRunning)) { fastest = false; calculateRoute(); }
        if (button({ 1048, 270, 214, 42 }, "Duong nhanh nhat", !taxiRunning)) { fastest = true; calculateRoute(); }
        if (button({ 850, 326, 412, 42 }, "Chay mo phong taxi", hasRoute && !taxiRunning)) {
            taxiRunning = true;
            taxiStarted = GetTime();
        }
        if (button({ 850, 382, 412, 38 }, "Dat lai", !taxiRunning)) {
            hasRoute = false; pathCount = 0; routeDistance = 0; routeTime = 0;
            status = "Da dat lai ket qua.";
        }
        DrawText("DIEU KHIEN", 846, 105, 20, RAYWHITE);
        DrawText("DIEM XUAT PHAT", 850, 126, 13, PBL_MUTED);
        const intersection& start = city.getIntersectionAt(startIndex);
        DrawText(TextFormat("%d - %s", start.getid_i(), start.getname_i().c_str()), 900, 153, 15, PBL_GREEN);
        DrawText("DIEM DEN", 850, 186, 13, PBL_MUTED);
        const intersection& finish = city.getIntersectionAt(endIndex);
        DrawText(TextFormat("%d - %s", finish.getid_i(), finish.getname_i().c_str()), 900, 213, 15, PBL_RED);
        DrawText("Nhan < hoac > de doi giao lo.", 850, 244, 13, PBL_MUTED);

        DrawText("KET QUA", 850, 444, 19, RAYWHITE);
        DrawText("Trang thai:", 850, 475, 14, PBL_MUTED);
        DrawText(status.c_str(), 850, 496, 14, hasRoute ? PBL_GREEN : RAYWHITE);
        if (hasRoute) {
            DrawText(TextFormat("So giao lo tren tuyen: %d", pathCount), 850, 529, 15, RAYWHITE);
            DrawText(TextFormat("Tong quang duong: %s", number(routeDistance).c_str()), 850, 556, 15, RAYWHITE);
            DrawText(TextFormat("Tong thoi gian: %s", number(routeTime).c_str()), 850, 583, 15, RAYWHITE);
            DrawText("Lo trinh:", 850, 616, 14, PBL_MUTED);
            std::string routeText;
            for (int i = 0; i < pathCount; ++i) {
                routeText += std::to_string(path[i]);
                if (i + 1 < pathCount) routeText += " -> ";
            }
            if (routeText.size() > 50) routeText = routeText.substr(0, 47) + "...";
            DrawText(routeText.c_str(), 850, 638, 14, PBL_YELLOW);
        } else {
            DrawText("Chua co ket qua. Hay tim duong.", 850, 529, 14, PBL_MUTED);
        }
        DrawText("Chu thich:", 850, 688, 13, PBL_MUTED);
        DrawCircle(860, 715, 6, PBL_GREEN); DrawText("Bat dau", 873, 708, 12, RAYWHITE);
        DrawCircle(960, 715, 6, PBL_RED); DrawText("Dich den", 973, 708, 12, RAYWHITE);
        DrawCircle(1064, 715, 6, PBL_YELLOW); DrawText("Tuyen/taxi", 1077, 708, 12, RAYWHITE);
        DrawText("ESC: thoat | Du lieu doc tu thu muc Data", 34, 779, 12, PBL_MUTED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
