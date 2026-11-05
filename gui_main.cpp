#include "raylib.h"
#include "BLL/route_service.h"
#include "DAL/text_map_repository.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <exception>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

namespace {
constexpr int SCREEN_W = 1280;
constexpr int SCREEN_H = 800;
const Rectangle MAP_VIEWPORT = { 32, 130, 760, 610 };
const Vector2 MAP_CENTER = { 370, 410 };
const Color PBL_BG = { 15, 23, 42, 255 };
const Color PBL_PANEL = { 25, 36, 58, 255 };
const Color PBL_ROAD = { 75, 91, 116, 255 };
const Color PBL_BLUE = { 59, 130, 246, 255 };
const Color PBL_PICKUP = { 244, 114, 182, 255 };
const Color PBL_GREEN = { 34, 197, 94, 255 };
const Color PBL_RED = { 239, 68, 68, 255 };
const Color PBL_YELLOW = { 250, 204, 21, 255 };
const Color PBL_MUTED = { 164, 178, 199, 255 };

struct ScreenPoint { float x; float y; };

enum class ManagedEntity { Intersection, Road };
enum class FormAction { Add, Edit, Delete };

bool button(Rectangle r, const char* label, bool active = true, bool selected = false) {
    Vector2 mouse = GetMousePosition();
    bool hover = CheckCollisionPointRec(mouse, r);
    Color fill = active ? (selected ? Color{ 37, 99, 160, 255 }
                                    : (hover ? Color{ 71, 85, 105, 255 } : Color{ 51, 65, 85, 255 }))
                        : Color{ 45, 55, 72, 255 };
    DrawRectangleRounded(r, 0.18f, 8, fill);
    DrawRectangleRoundedLines(r, 0.18f, 8, active ? PBL_BLUE : Color{ 65, 75, 90, 255 });
    int tw = MeasureText(label, 16);
    DrawText(label, (int)(r.x + (r.width - tw) / 2), (int)(r.y + (r.height - 16) / 2), 16,
             active ? RAYWHITE : PBL_MUTED);
    return active && hover && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

string number(double value) {
    ostringstream out;
    out << fixed << setprecision(2) << value;
    return out.str();
}

bool parseInt(const string& text, int& value) {
    istringstream input(text);
    char extra;
    return (input >> value) && !(input >> extra);
}

bool parseDouble(const string& text, double& value) {
    istringstream input(text);
    char extra;
    return (input >> value) && !(input >> extra) && isfinite(value);
}
}

int runGui() {
    TextMapRepository repository;
    RouteService routeService(repository);
    if (!routeService.isReady()) {
        InitWindow(700, 180, "PBL2 - Loi du lieu du du lieu");
        BeginDrawing();
        ClearBackground(PBL_BG);
        DrawText("Khong doc duoc du lieu ban do!", 24, 30, 22, PBL_RED);
        DrawText(routeService.getLoadError().c_str(), 24, 75, 14, RAYWHITE);
        EndDrawing();
        WaitTime(3.0);
        CloseWindow();
        return 1;
    }

    const MapView& mapView = routeService.getMapView();
    int n = static_cast<int>(mapView.intersections.size());
    if (n < 2) return 1;
    vector<int> routePath, routeRoadIds;
    int firstPathCount = 0;
    int taxiIndex = 0;
    int pickupIndex = min(7, n - 1);
    int destinationIndex = min(14, n - 1);
    int selectionTarget = 0;
    bool fastest = false;
    bool hasRoute = false;
    bool taxiRunning = false;
    bool managementOpen = false;
    ManagedEntity managedEntity = ManagedEntity::Intersection;
    FormAction formAction = FormAction::Add;
    vector<string> formFields;
    int activeField = 0;
    int managementListOffset = 0;
    int selectedEntityIndex = -1;
    string managementMessage;
    float mapZoom = 1.0f;
    Vector2 mapPan{ 0.0f, 0.0f };
    double taxiStarted = 0.0;
    double routeDistance = 0.0;
    double routeTime = 0.0;
    double firstDistance = 0.0;
    double firstTime = 0.0;
    double secondDistance = 0.0;
    double secondTime = 0.0;
    string status = "Chon taxi, diem don va dich tren ban do.";

    auto calculateRoute = [&]() {
        firstPathCount = 0;
        routePath.clear();
        routeRoadIds.clear();
        const RouteResult result = routeService.findTaxiRoute(
            mapView.intersections[taxiIndex].id,
            mapView.intersections[pickupIndex].id,
            mapView.intersections[destinationIndex].id,
            fastest ? RouteMode::Fastest : RouteMode::Shortest);
        taxiRunning = false;
        hasRoute = result.success;
        if (!hasRoute) {
            routeDistance = routeTime = 0.0;
            firstDistance = firstTime = 0.0;
            secondDistance = secondTime = 0.0;
            status = result.error;
            return;
        }

        routePath = result.intersectionIds;
        routeRoadIds = result.roadIds;
        firstPathCount = static_cast<int>(result.firstLegEdgeCount) + 1;
        firstDistance = result.firstLegDistance;
        firstTime = result.firstLegTime;
        secondDistance = result.secondLegDistance;
        secondTime = result.secondLegTime;
        routeDistance = result.totalDistance;
        routeTime = result.totalTime;
        status = fastest ? "Da tim hai chang nhanh nhat." : "Da tim hai chang ngan nhat.";
    };

    auto beginForm = [&](FormAction action) {
        formAction = action;
        formFields.clear();
        if (managedEntity == ManagedEntity::Intersection) {
            formFields = action == FormAction::Delete
                ? vector<string>{"ID giao lo"}
                : vector<string>{"ID giao lo", "Ten", "Toa do X", "Toa do Y"};
        } else {
            formFields = action == FormAction::Delete
                ? vector<string>{"ID duong"}
                : vector<string>{"ID duong", "ID dau", "ID cuoi", "Thoi gian", "Khoang cach"};
        }
        activeField = 0;
        managementMessage.clear();
    };

    auto loadSelectedIntoForm = [&]() {
        if (selectedEntityIndex < 0) return;
        if (managedEntity == ManagedEntity::Intersection) {
            if (selectedEntityIndex >= static_cast<int>(mapView.intersections.size())) return;
            const IntersectionView& item = mapView.intersections[selectedEntityIndex];
            formFields[0] = to_string(item.id);
            if (formAction != FormAction::Delete) {
                formFields[1] = item.name;
                formFields[2] = number(item.x);
                formFields[3] = number(item.y);
            }
        } else {
            if (selectedEntityIndex >= static_cast<int>(mapView.roads.size())) return;
            const RoadView& item = mapView.roads[selectedEntityIndex];
            formFields[0] = to_string(item.id);
            if (formAction != FormAction::Delete) {
                formFields[1] = to_string(item.startId);
                formFields[2] = to_string(item.endId);
                formFields[3] = number(item.time);
                formFields[4] = number(item.distance);
            }
        }
    };

    auto submitForm = [&]() {
        string error;
        int id = 0;
        bool success = false;
        if (formFields.empty() || !parseInt(formFields[0], id) || id <= 0) {
            managementMessage = "ID phai la so nguyen duong.";
            return;
        }

        if (managedEntity == ManagedEntity::Intersection) {
            if (formAction == FormAction::Delete) {
                success = routeService.removeIntersection(id, error);
            } else {
                double x = 0.0, y = 0.0;
                if (!parseDouble(formFields[2], x) || !parseDouble(formFields[3], y)) {
                    managementMessage = "Toa do X va Y phai la so hop le.";
                    return;
                }
                IntersectionView item{id, formFields[1], x, y};
                success = formAction == FormAction::Add
                    ? routeService.addIntersection(item, error)
                    : routeService.updateIntersection(id, item, error);
            }
        } else {
            if (formAction == FormAction::Delete) {
                success = routeService.removeRoad(id, error);
            } else {
                int startId = 0, endId = 0;
                double time = 0.0, distance = 0.0;
                if (!parseInt(formFields[1], startId) || !parseInt(formFields[2], endId) ||
                    !parseDouble(formFields[3], time) || !parseDouble(formFields[4], distance)) {
                    managementMessage = "ID dau/cuoi hoac thoi gian/khoang cach khong hop le.";
                    return;
                }
                RoadView item{id, startId, endId, time, distance};
                success = formAction == FormAction::Add
                    ? routeService.addRoad(item, error)
                    : routeService.updateRoad(id, item, error);
            }
        }

        if (success) {
            n = static_cast<int>(mapView.intersections.size());
            taxiIndex = min(taxiIndex, n - 1);
            pickupIndex = min(pickupIndex, n - 1);
            destinationIndex = min(destinationIndex, n - 1);
            hasRoute = false;
            routePath.clear();
            routeRoadIds.clear();
            firstPathCount = 0;
            status = "Du lieu ban do vua duoc cap nhat.";
            beginForm(formAction);
            managementMessage = "Da cap nhat ban do va luu vao file.";
        } else {
            managementMessage = error;
            n = static_cast<int>(mapView.intersections.size());
            taxiIndex = min(taxiIndex, n - 1);
            pickupIndex = min(pickupIndex, n - 1);
            destinationIndex = min(destinationIndex, n - 1);
            hasRoute = false;
            routePath.clear();
            routeRoadIds.clear();
            firstPathCount = 0;
        }
    };

    auto pointFor = [&](int id) -> ScreenPoint {
        const double minX = 10.0, maxX = 90.0, minY = 15.0, maxY = 100.0;
        const IntersectionView* item = nullptr;
        for (const IntersectionView& candidate : mapView.intersections) {
            if (candidate.id == id) {
                item = &candidate;
                break;
            }
        }
        if (!item) return { 0, 0 };
        float baseX = 45.0f + (float)((item->x - minX) / (maxX - minX) * 650.0);
        float baseY = 690.0f - (float)((item->y - minY) / (maxY - minY) * 560.0);
        return { MAP_CENTER.x + (baseX - MAP_CENTER.x) * mapZoom + mapPan.x,
                 MAP_CENTER.y + (baseY - MAP_CENTER.y) * mapZoom + mapPan.y };
    };

    auto zoomAt = [&](float newZoom, Vector2 anchor) {
        newZoom = clamp(newZoom, 0.75f, 4.0f);
        float ratio = newZoom / mapZoom;
        mapPan.x = anchor.x + (MAP_CENTER.x - anchor.x + mapPan.x) * ratio - MAP_CENTER.x;
        mapPan.y = anchor.y + (MAP_CENTER.y - anchor.y + mapPan.y) * ratio - MAP_CENTER.y;
        mapZoom = newZoom;
    };

    InitWindow(SCREEN_W, SCREEN_H, "PBL2 | Ban do va tim duong Dijkstra");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        Vector2 mouse = GetMousePosition();
        if (!managementOpen && CheckCollisionPointRec(mouse, MAP_VIEWPORT)) {
            float wheel = GetMouseWheelMove();
            if (wheel != 0.0f) zoomAt(mapZoom * pow(1.2f, wheel), mouse);
            if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
                Vector2 delta = GetMouseDelta();
                mapPan.x += delta.x;
                mapPan.y += delta.y;
            }
        }

        BeginDrawing();
        ClearBackground(PBL_BG);
        DrawText("PBL2 - MO PHONG BAN DO GIAO THONG", 32, 22, 27, RAYWHITE);
        DrawText("Raylib + OOP C++ + thuat toan Dijkstra", 34, 55, 16, PBL_MUTED);
        DrawRectangleRounded({ 22, 88, 780, 686 }, 0.025f, 8, PBL_PANEL);
        DrawText("BAN DO GIAO LO", 42, 103, 17, PBL_MUTED);
        if (button({ 690, 97, 32, 30 }, "+")) zoomAt(mapZoom * 1.25f, MAP_CENTER);
        if (button({ 728, 97, 32, 30 }, "-")) zoomAt(mapZoom / 1.25f, MAP_CENTER);
        if (button({ 766, 97, 32, 30 }, "100%")) {
            mapZoom = 1.0f;
            mapPan = { 0.0f, 0.0f };
        }

        BeginScissorMode((int)MAP_VIEWPORT.x, (int)MAP_VIEWPORT.y,
                         (int)MAP_VIEWPORT.width, (int)MAP_VIEWPORT.height);
        // Draw all roads first; selected route roads are highlighted.
        for (const RoadView& r : mapView.roads) {
            ScreenPoint a = pointFor(r.startId);
            ScreenPoint b = pointFor(r.endId);
            bool firstLeg = false;
            bool secondLeg = false;
            if (hasRoute) {
                const int firstLegEdges = max(0, firstPathCount - 1);
                for (int j = 0; j < static_cast<int>(routeRoadIds.size()); ++j) {
                    if (routeRoadIds[j] == r.id) {
                        if (j < firstLegEdges) firstLeg = true;
                        else secondLeg = true;
                    }
                }
            }
            Color roadColor = firstLeg ? PBL_BLUE : (secondLeg ? PBL_YELLOW : PBL_ROAD);
            DrawLineEx({ a.x, a.y }, { b.x, b.y }, firstLeg || secondLeg ? 5.0f : 2.0f, roadColor);
            if (firstLeg || secondLeg) {
                char label[32];
                snprintf(label, sizeof(label), "%.1f", r.distance);
                DrawText(label, (int)((a.x + b.x) / 2 + 3), (int)((a.y + b.y) / 2 - 14), 10,
                         firstLeg ? PBL_BLUE : PBL_YELLOW);
            }
        }

        // Taxi moves along the chosen path, interpolating between intersections.
        ScreenPoint taxiPoint{ 0, 0 };
        bool drawTaxi = false;
        if (taxiRunning && hasRoute && !routePath.empty()) {
            double elapsed = GetTime() - taxiStarted;
            double duration = max(1.0, (double)(routePath.size() - 1) * 1.1);
            double progress = elapsed / duration;
            double segmentPos = min(progress, 0.9999) * max(1, static_cast<int>(routePath.size()) - 1);
            int segment = min((int)segmentPos, max(0, static_cast<int>(routePath.size()) - 2));
            if (progress >= 1.0) {
                taxiRunning = false;
                status = "Taxi da dua khach den noi.";
            } else if (segment < max(0, firstPathCount - 1)) {
                status = "Taxi dang den diem don khach.";
            } else {
                status = "Taxi dang cho khach.";
            }
            float t = (float)(segmentPos - segment);
            ScreenPoint a = pointFor(routePath[segment]);
            ScreenPoint b = pointFor(routePath[min(segment + 1, static_cast<int>(routePath.size()) - 1)]);
            taxiPoint = { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t };
            drawTaxi = true;
        }

        bool intersectionSelected = false;
        for (int i = 0; i < n; ++i) {
            const IntersectionView& node = mapView.intersections[i];
            ScreenPoint p = pointFor(node.id);
            Color c = PBL_BLUE;
            if (i == taxiIndex) c = PBL_GREEN;
            if (i == pickupIndex) c = PBL_PICKUP;
            if (i == destinationIndex) c = PBL_RED;
            if (hasRoute) {
                for (size_t j = 1; j + 1 < routePath.size(); ++j) {
                    if (routePath[j] == node.id && i != pickupIndex) c = PBL_YELLOW;
                }
            }
            bool namedIntersection = !node.name.empty();
            bool selectedIntersection = i == taxiIndex || i == pickupIndex || i == destinationIndex;
            Vector2 mouse = GetMousePosition();
            float dx = mouse.x - p.x;
            float dy = mouse.y - p.y;
            if (!managementOpen && !intersectionSelected && !taxiRunning && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                CheckCollisionPointRec(mouse, MAP_VIEWPORT) &&
                dx * dx + dy * dy <= 18.0f * 18.0f) {
                if (selectionTarget == 0) taxiIndex = i;
                else if (selectionTarget == 1) pickupIndex = i;
                else destinationIndex = i;
                hasRoute = false;
                status = "Da chon giao lo. Bam tim duong de cap nhat.";
                intersectionSelected = true;
            }
            float nodeRadius = namedIntersection || selectedIntersection ? 12.0f : 5.0f;
            DrawCircle((int)p.x, (int)p.y, nodeRadius, namedIntersection || selectedIntersection ? c : PBL_MUTED);
            DrawCircleLines((int)p.x, (int)p.y, nodeRadius, RAYWHITE);
            if (namedIntersection) {
                char idText[12];
                snprintf(idText, sizeof(idText), "%d", node.id);
                DrawText(idText, (int)p.x - MeasureText(idText, 12) / 2, (int)p.y - 6, 12, RAYWHITE);
                string name = node.name;
                if (name.size() > 20) name = name.substr(0, 18) + "..";
                DrawText(name.c_str(), (int)p.x + 14, (int)p.y - 8, 11, RAYWHITE);
            }
        }
        if (drawTaxi) {
            DrawCircle((int)taxiPoint.x, (int)taxiPoint.y, 8, PBL_YELLOW);
            DrawCircleLines((int)taxiPoint.x, (int)taxiPoint.y, 8, BLACK);
        }
        EndScissorMode();
        DrawText("Con lan: zoom | Chuot phai: keo ban do", 42, 750, 12, PBL_MUTED);

        // Right control panel
        DrawRectangleRounded({ 820, 88, 440, 686 }, 0.025f, 8, PBL_PANEL);
        if (button({ 850, 145, 126, 36 }, "Vi tri taxi", !managementOpen, selectionTarget == 0)) selectionTarget = 0;
        if (button({ 986, 145, 126, 36 }, "Diem don", !managementOpen, selectionTarget == 1)) selectionTarget = 1;
        if (button({ 1122, 145, 126, 36 }, "Diem den", !managementOpen, selectionTarget == 2)) selectionTarget = 2;
        if (button({ 850, 276, 198, 42 }, "Duong ngan nhat", !taxiRunning && !managementOpen, !fastest)) { fastest = false; calculateRoute(); }
        if (button({ 1058, 276, 190, 42 }, "Duong nhanh nhat", !taxiRunning && !managementOpen, fastest)) { fastest = true; calculateRoute(); }
        if (button({ 850, 328, 398, 42 }, "Chay mo phong taxi", hasRoute && !taxiRunning && !managementOpen)) {
            taxiRunning = true;
            taxiStarted = GetTime();
            status = firstPathCount > 1 ? "Taxi dang den diem don khach." : "Taxi dang cho khach.";
        }
        if (button({ 850, 380, 398, 38 }, "Dat lai", !taxiRunning && !managementOpen)) {
            hasRoute = false;
            firstPathCount = 0;
            routePath.clear();
            routeRoadIds.clear();
            routeDistance = 0;
            routeTime = 0;
            status = "Da xoa ket qua tim duong.";
        }
        DrawText("DIEU PHOI CHUYEN TAXI", 846, 105, 20, RAYWHITE);
        DrawText("Bam nut vai tro, sau do chon giao lo tren ban do.", 850, 190, 12, PBL_MUTED);
        const IntersectionView& taxiPosition = mapView.intersections[taxiIndex];
        const IntersectionView& pickup = mapView.intersections[pickupIndex];
        const IntersectionView& destination = mapView.intersections[destinationIndex];
        DrawText(TextFormat("Taxi: %d - %s", taxiPosition.id, taxiPosition.name.c_str()), 850, 210, 14, PBL_GREEN);
        DrawText(TextFormat("Don khach: %d - %s", pickup.id, pickup.name.c_str()), 850, 232, 14, PBL_PICKUP);
        DrawText(TextFormat("Diem den: %d - %s", destination.id, destination.name.c_str()), 850, 254, 14, PBL_RED);

        DrawText("KET QUA CHUYEN DI", 850, 438, 19, RAYWHITE);
        DrawText("Trang thai:", 850, 466, 14, PBL_MUTED);
        DrawText(status.c_str(), 850, 487, 13, hasRoute ? PBL_GREEN : RAYWHITE);
        if (hasRoute) {
            DrawText(TextFormat("Taxi -> don: %s km | %s phut", number(firstDistance).c_str(), number(firstTime).c_str()), 850, 520, 14, RAYWHITE);
            DrawText(TextFormat("Don -> dich: %s km | %s phut", number(secondDistance).c_str(), number(secondTime).c_str()), 850, 546, 14, RAYWHITE);
            DrawText(TextFormat("Tong: %s km | %s phut", number(routeDistance).c_str(), number(routeTime).c_str()), 850, 574, 15, PBL_GREEN);
            DrawText("Lo trinh:", 850, 610, 14, PBL_MUTED);
            string routeText;
            for (size_t i = 0; i < routePath.size(); ++i) {
                routeText += to_string(routePath[i]);
                if (i + 1 < routePath.size()) routeText += " -> ";
            }
            if (routeText.size() > 50) routeText = routeText.substr(0, 47) + "...";
            DrawText(routeText.c_str(), 850, 632, 14, PBL_YELLOW);
        } else {
            DrawText("Chua co ket qua. Hay tim duong.", 850, 520, 14, PBL_MUTED);
        }
        DrawText("Chu thich:", 850, 688, 13, PBL_MUTED);
        DrawCircle(860, 715, 6, PBL_GREEN); DrawText("Taxi", 873, 708, 12, RAYWHITE);
        DrawCircle(930, 715, 6, PBL_PICKUP); DrawText("Diem don", 943, 708, 12, RAYWHITE);
        DrawCircle(1035, 715, 6, PBL_RED); DrawText("Diem den", 1048, 708, 12, RAYWHITE);
        DrawCircle(860, 742, 6, PBL_BLUE); DrawText("Chang taxi den don", 873, 735, 12, RAYWHITE);
        DrawCircle(1050, 742, 6, PBL_YELLOW); DrawText("Chang cho khach", 1063, 735, 12, RAYWHITE);
        if (button({ 1030, 28, 220, 38 }, "Quan ly du lieu", !taxiRunning)) {
            managementOpen = true;
            managedEntity = ManagedEntity::Intersection;
            selectedEntityIndex = -1;
            managementListOffset = 0;
            beginForm(FormAction::Add);
        }

        if (managementOpen) {
            DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Color{0, 0, 0, 190});
            DrawRectangleRounded({ 170, 92, 940, 620 }, 0.04f, 10, PBL_PANEL);
            DrawText("QUAN LY GIAO LO VA DUONG", 200, 118, 24, RAYWHITE);
            DrawText("Thay doi duoc luu vao Data/ ngay sau khi thuc hien.",
                     200, 151, 14, PBL_MUTED);

            if (button({ 200, 185, 190, 38 }, "Giao lo", true,
                       managedEntity == ManagedEntity::Intersection)) {
                managedEntity = ManagedEntity::Intersection;
                selectedEntityIndex = -1;
                managementListOffset = 0;
                beginForm(FormAction::Add);
            }
            if (button({ 400, 185, 190, 38 }, "Duong", true,
                       managedEntity == ManagedEntity::Road)) {
                managedEntity = ManagedEntity::Road;
                selectedEntityIndex = -1;
                managementListOffset = 0;
                beginForm(FormAction::Add);
            }

            DrawText(managedEntity == ManagedEntity::Intersection
                         ? TextFormat("DANH SACH GIAO LO (%i)", static_cast<int>(mapView.intersections.size()))
                         : TextFormat("DANH SACH DUONG (%i)", static_cast<int>(mapView.roads.size())),
                     200, 245, 16, RAYWHITE);
            DrawText("ID | Thong tin", 205, 273, 12, PBL_MUTED);

            const int entityCount = managedEntity == ManagedEntity::Intersection
                ? static_cast<int>(mapView.intersections.size())
                : static_cast<int>(mapView.roads.size());
            const int visibleRows = 9;
            managementListOffset = clamp(managementListOffset, 0, max(0, entityCount - visibleRows));
            const Rectangle listBounds = { 195, 292, 390, 310 };
            if (CheckCollisionPointRec(mouse, listBounds)) {
                managementListOffset = clamp(
                    managementListOffset - static_cast<int>(GetMouseWheelMove()),
                    0, max(0, entityCount - visibleRows));
            }
            BeginScissorMode((int)listBounds.x, (int)listBounds.y,
                             (int)listBounds.width, (int)listBounds.height);
            for (int row = 0; row < visibleRows; ++row) {
                int index = managementListOffset + row;
                if (index >= entityCount) break;
                Rectangle rowBounds = { 200, 296.0f + row * 33.0f, 380, 30 };
                bool selected = index == selectedEntityIndex;
                DrawRectangleRounded(rowBounds, 0.12f, 5,
                    selected ? Color{ 37, 99, 160, 255 } : Color{ 34, 45, 64, 255 });
                if (managedEntity == ManagedEntity::Intersection) {
                    const IntersectionView& item = mapView.intersections[index];
                    string label = to_string(item.id) + " | " + item.name +
                        " (" + number(item.x) + ", " + number(item.y) + ")";
                    if (label.size() > 48) label = label.substr(0, 45) + "...";
                    DrawText(label.c_str(), 210, 303 + row * 33, 13, RAYWHITE);
                } else {
                    const RoadView& item = mapView.roads[index];
                    string label = to_string(item.id) + " | " + to_string(item.startId) +
                        " <-> " + to_string(item.endId) + " | " +
                        number(item.distance) + " km, " + number(item.time) + " phut";
                    DrawText(label.c_str(), 210, 303 + row * 33, 13, RAYWHITE);
                }
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mouse, rowBounds)) {
                    selectedEntityIndex = index;
                    if (formAction != FormAction::Add) loadSelectedIntoForm();
                }
            }
            EndScissorMode();
            DrawText("Cuon chuot de xem danh sach", 200, 610, 12, PBL_MUTED);

            if (button({ 610, 237, 130, 36 }, "Them", true, formAction == FormAction::Add))
                beginForm(FormAction::Add);
            if (button({ 750, 237, 130, 36 }, "Sua", true, formAction == FormAction::Edit)) {
                beginForm(FormAction::Edit);
                loadSelectedIntoForm();
            }
            if (button({ 890, 237, 130, 36 }, "Xoa", true, formAction == FormAction::Delete)) {
                beginForm(FormAction::Delete);
                loadSelectedIntoForm();
            }

            static const char* intersectionLabels[] = {"ID giao lo", "Ten", "Toa do X", "Toa do Y"};
            static const char* roadLabels[] = {"ID duong", "ID dau", "ID cuoi", "Thoi gian", "Khoang cach"};
            for (size_t i = 0; i < formFields.size(); ++i) {
                float y = 302.0f + static_cast<float>(i) * 52.0f;
                const char* label = managedEntity == ManagedEntity::Intersection
                    ? intersectionLabels[i] : roadLabels[i];
                DrawText(label, 610, static_cast<int>(y), 14, PBL_MUTED);
                Rectangle field = { 730, y - 5, 340, 34 };
                DrawRectangleRounded(field, 0.12f, 6,
                    static_cast<int>(i) == activeField ? Color{ 38, 55, 82, 255 }
                                                       : Color{ 34, 45, 64, 255 });
                DrawRectangleRoundedLines(field, 0.12f, 6,
                    static_cast<int>(i) == activeField ? PBL_BLUE : PBL_MUTED);
                DrawText(formFields[i].c_str(), 742, static_cast<int>(y + 3), 16, RAYWHITE);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) &&
                    CheckCollisionPointRec(mouse, field)) {
                    activeField = static_cast<int>(i);
                }
            }

            if (!formFields.empty()) {
                int key = GetCharPressed();
                while (key > 0) {
                    if (activeField >= 0 && activeField < static_cast<int>(formFields.size()) &&
                        formFields[activeField].size() < 60 && key >= 32 && key <= 125 &&
                        key != '|') {
                        bool numericField = activeField == 0 ||
                            managedEntity == ManagedEntity::Road ||
                            (formAction != FormAction::Delete &&
                             (activeField == 2 || activeField == 3));
                        bool numericCharacter = (key >= '0' && key <= '9') || key == '.' ||
                            (key == '-' && formFields[activeField].empty());
                        if (!numericField || numericCharacter) {
                            formFields[activeField].push_back(static_cast<char>(key));
                        }
                    }
                    key = GetCharPressed();
                }
                if (IsKeyPressed(KEY_BACKSPACE) && activeField >= 0 &&
                    activeField < static_cast<int>(formFields.size()) &&
                    !formFields[activeField].empty()) {
                    formFields[activeField].pop_back();
                }
                if (IsKeyPressed(KEY_TAB))
                    activeField = (activeField + 1) % static_cast<int>(formFields.size());
                if (IsKeyPressed(KEY_ENTER)) submitForm();
            }

            if (button({ 610, 592, 180, 42 }, "Luu thay doi"))
                submitForm();
            if (button({ 890, 592, 140, 42 }, "Dong")) managementOpen = false;
            if (!managementMessage.empty())
                DrawText(managementMessage.c_str(), 610, 646, 13,
                         managementMessage.find("Da cap nhat") == 0 ? PBL_GREEN : PBL_RED);
            DrawText("Click vao o de nhap | Tab chuyen o | Enter de luu",
                     610, 672, 12, PBL_MUTED);
        }
        DrawText("ESC: thoat | Du lieu doc tu thu muc Data", 34, 779, 12, PBL_MUTED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

int main() {
    try {
        return runGui();
    } catch (const exception& error) {
        cerr << "Loi ngoai le: " << error.what() << endl;
    } catch (...) {
        cerr << "Loi khong xac dinh." << endl;
    }
    return 1;
}
