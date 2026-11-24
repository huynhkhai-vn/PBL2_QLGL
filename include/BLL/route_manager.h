#ifndef ROUTE_SERVICE_H
#define ROUTE_SERVICE_H

#include "DAL/map_data_access.h"
#include "dijkstra.h"
#include <cmath>
#include <string>

using namespace std;

const int ROUTE_SERVICE_MAX_INTERSECTIONS = 100;
const int ROUTE_SERVICE_MAX_ROADS = 100;
const int ROUTE_SERVICE_MAX_ROUTE_INTERSECTIONS =
    ROUTE_SERVICE_MAX_INTERSECTIONS * 2 - 1;
const int ROUTE_SERVICE_MAX_ROUTE_ROADS =
    ROUTE_SERVICE_MAX_INTERSECTIONS * 2 - 2;

enum class RouteMode {
    Shortest,
    Fastest
};

struct IntersectionView {
    int id;
    string name;
    double x;
    double y;
};

struct RoadView {
    int id;
    int startId;
    int endId;
    double time;
    double distance;
};

struct MapView {
    IntersectionView intersections[ROUTE_SERVICE_MAX_INTERSECTIONS]{};
    RoadView roads[ROUTE_SERVICE_MAX_ROADS]{};
    int intersectionCount = 0;
    int roadCount = 0;
};

struct RouteResult {
    bool success = false;
    string error;
    int intersectionIds[ROUTE_SERVICE_MAX_ROUTE_INTERSECTIONS]{};
    int roadIds[ROUTE_SERVICE_MAX_ROUTE_ROADS]{};
    int intersectionCount = 0;
    int roadCount = 0;
    int firstLegEdgeCount = 0;
    double firstLegDistance = 0.0;
    double firstLegTime = 0.0;
    double secondLegDistance = 0.0;
    double secondLegTime = 0.0;
    double totalDistance = 0.0;
    double totalTime = 0.0;
};

class RouteManager {
private:
    const MapDataAccess& dataAccess;
    map city;
    MapView mapView;
    string loadError;
    bool ready;

    void createMapView();

public:
    RouteManager(const MapDataAccess& dataAccess);

    bool isReady() const;
    const string& getLoadError() const;
    const MapView& getMapView() const;
    RouteResult findTaxiRoute(int taxiId, int pickupId, int destinationId, RouteMode mode) const;
    bool addIntersection(const IntersectionView& item, string& error);
    bool updateIntersection(int id, const IntersectionView& item, string& error);
    bool removeIntersection(int id, string& error);
    bool addRoad(const RoadView& item, string& error);
    bool updateRoad(int id, const RoadView& item, string& error);
    bool removeRoad(int id, string& error);
};

RouteManager::RouteManager(const MapDataAccess& dataAccess)
    : dataAccess(dataAccess), city(), mapView(), loadError(),
      ready(dataAccess.load(city, loadError)) {
    if (ready) {
        createMapView();
    }
}

bool RouteManager::isReady() const {
    return ready;
}

const string& RouteManager::getLoadError() const {
    return loadError;
}

const MapView& RouteManager::getMapView() const {
    return mapView;
}

void RouteManager::createMapView() {
    mapView.intersectionCount = 0;
    mapView.roadCount = 0;
    for (int i = 0; i < city.get_num_i(); ++i) {
        const intersection& item = city.getIntersectionAt(i);
        mapView.intersections[mapView.intersectionCount++] =
            {item.getid_i(), item.getname_i(), item.getx(), item.gety()};
    }

    for (int i = 0; i < city.get_num_r(); ++i) {
        const road& item = city.getRoadAt(i);
        mapView.roads[mapView.roadCount++] =
            {item.getid_r(), item.getstart_id(), item.getend_id(),
             item.gettime(), item.getdistance()};
    }
}

bool RouteManager::addIntersection(const IntersectionView& item, string& error) {
    if (item.id <= 0 || item.name.empty() || item.name.find_first_of("|\r\n") != string::npos ||
        !isfinite(item.x) || !isfinite(item.y)) {
        error = "Thong tin giao lo khong hop le.";
        return false;
    }
    if (!city.add_i(intersection(item.id, item.name, item.x, item.y))) {
        error = "ID giao lo bi trung hoac ban do da day.";
        return false;
    }
    createMapView();
    return dataAccess.save(city, error);
}

bool RouteManager::updateIntersection(
    int id, const IntersectionView& item, string& error) {
    if (item.name.empty() || item.name.find_first_of("|\r\n") != string::npos ||
        !isfinite(item.x) || !isfinite(item.y)) {
        error = "Thong tin giao lo khong hop le.";
        return false;
    }
    if (!city.update_i(id, item.name, item.x, item.y)) {
        error = "Khong tim thay giao lo can sua.";
        return false;
    }
    createMapView();
    return dataAccess.save(city, error);
}

bool RouteManager::removeIntersection(int id, string& error) {
    if (city.find_i(id) == -1) {
        error = "Khong tim thay giao lo can xoa.";
        return false;
    }
    if (city.get_num_i() <= 1) {
        error = "Ban do phai con it nhat mot giao lo.";
        return false;
    }
    city.remove_i(id);
    createMapView();
    return dataAccess.save(city, error);
}

bool RouteManager::addRoad(const RoadView& item, string& error) {
    if (item.id <= 0 || item.startId == item.endId ||
        !isfinite(item.time) || !isfinite(item.distance) ||
        item.time < 0 || item.distance < 0) {
        error = "Thong tin duong khong hop le.";
        return false;
    }
    if (!city.add_r(road(item.id, item.startId, item.endId, item.time, item.distance))) {
        error = "ID duong bi trung, giao lo khong ton tai hoac ban do da day.";
        return false;
    }
    createMapView();
    return dataAccess.save(city, error);
}

bool RouteManager::updateRoad(int id, const RoadView& item, string& error) {
    if (item.startId == item.endId || !isfinite(item.time) ||
        !isfinite(item.distance) || item.time < 0 || item.distance < 0) {
        error = "Thong tin duong khong hop le.";
        return false;
    }
    if (!city.update_r(id, item.startId, item.endId, item.time, item.distance)) {
        error = "Khong the sua duong: hay kiem tra ID va hai giao lo dau-cuoi.";
        return false;
    }
    createMapView();
    return dataAccess.save(city, error);
}

bool RouteManager::removeRoad(int id, string& error) {
    if (city.find_r(id) == -1) {
        error = "Khong tim thay duong can xoa.";
        return false;
    }
    city.remove_r(id);
    createMapView();
    return dataAccess.save(city, error);
}

RouteResult RouteManager::findTaxiRoute(
    int taxiId, int pickupId, int destinationId, RouteMode mode) const {
    RouteResult result;
    if (!ready) {
        result.error = loadError.empty() ? "Du lieu ban do chua san sang." : loadError;
        return result;
    }

    if (city.find_i(taxiId) == -1 || city.find_i(pickupId) == -1 ||
        city.find_i(destinationId) == -1) {
        result.error = "Giao lo duoc chon khong ton tai.";
        return result;
    }

    int firstPath[ROUTE_SERVICE_MAX_INTERSECTIONS]{};
    int firstRoadIds[ROUTE_SERVICE_MAX_INTERSECTIONS]{};
    int secondPath[ROUTE_SERVICE_MAX_INTERSECTIONS]{};
    int secondRoadIds[ROUTE_SERVICE_MAX_INTERSECTIONS]{};
    int firstPathCount = 0;
    int secondPathCount = 0;
    dijkstra algorithm;

    if (mode == RouteMode::Fastest) {
        algorithm.fastestPath(city, taxiId, pickupId, firstPath,
                              firstPathCount, firstRoadIds, false);
    } else {
        algorithm.shortestPath(city, taxiId, pickupId, firstPath,
                               firstPathCount, firstRoadIds, false);
    }
    if (firstPathCount == 0) {
        result.error = "Khong co duong tu taxi den diem don.";
        return result;
    }

    if (mode == RouteMode::Fastest) {
        algorithm.fastestPath(city, pickupId, destinationId, secondPath,
                              secondPathCount, secondRoadIds, false);
    } else {
        algorithm.shortestPath(city, pickupId, destinationId, secondPath,
                               secondPathCount, secondRoadIds, false);
    }
    if (secondPathCount == 0) {
        result.error = "Khong co duong tu diem don den dich.";
        return result;
    }

    for (int i = 0; i < firstPathCount; ++i) {
        result.intersectionIds[result.intersectionCount++] = firstPath[i];
    }
    for (int i = 0; i < firstPathCount - 1; ++i) {
        result.roadIds[result.roadCount++] = firstRoadIds[i];
    }
    for (int i = 1; i < secondPathCount; ++i) {
        result.intersectionIds[result.intersectionCount++] = secondPath[i];
    }
    for (int i = 0; i < secondPathCount - 1; ++i) {
        result.roadIds[result.roadCount++] = secondRoadIds[i];
    }
    result.firstLegEdgeCount = firstPathCount - 1;

    for (int i = 0; i < result.roadCount; ++i) {
        double distance = city.getRoadDistanceById(
            result.roadIds[i], result.intersectionIds[i], result.intersectionIds[i + 1]);
        double time = city.getRoadTimeById(
            result.roadIds[i], result.intersectionIds[i], result.intersectionIds[i + 1]);
        if (distance < 0.0 || time < 0.0) {
            result.intersectionCount = 0;
            result.roadCount = 0;
            result.error = "Khong doc duoc thong tin duong trong lo trinh.";
            return result;
        }

        result.totalDistance += distance;
        result.totalTime += time;
        if (i < result.firstLegEdgeCount) {
            result.firstLegDistance += distance;
            result.firstLegTime += time;
        } else {
            result.secondLegDistance += distance;
            result.secondLegTime += time;
        }
    }

    result.success = true;
    return result;
}

#endif
