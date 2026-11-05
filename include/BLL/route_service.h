#ifndef ROUTE_SERVICE_H
#define ROUTE_SERVICE_H

#include "BLL/map_repository.h"
#include "dijkstra.h"
#include <cmath>
#include <cstddef>
#include <string>
#include <vector>

using namespace std;

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
    vector<IntersectionView> intersections;
    vector<RoadView> roads;
};

struct RouteResult {
    bool success = false;
    string error;
    vector<int> intersectionIds;
    vector<int> roadIds;
    size_t firstLegEdgeCount = 0;
    double firstLegDistance = 0.0;
    double firstLegTime = 0.0;
    double secondLegDistance = 0.0;
    double secondLegTime = 0.0;
    double totalDistance = 0.0;
    double totalTime = 0.0;
};

class RouteService {
private:
    const IMapRepository& repository;
    map city;
    MapView mapView;
    string loadError;
    bool ready;

    void createMapView();

public:
    explicit RouteService(const IMapRepository& repository);

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

RouteService::RouteService(const IMapRepository& repository)
    : repository(repository), city(), mapView(), loadError(),
      ready(repository.load(city, loadError)) {
    if (ready) {
        createMapView();
    }
}

bool RouteService::isReady() const {
    return ready;
}

const string& RouteService::getLoadError() const {
    return loadError;
}

const MapView& RouteService::getMapView() const {
    return mapView;
}

void RouteService::createMapView() {
    mapView.intersections.clear();
    mapView.roads.clear();
    mapView.intersections.reserve(static_cast<size_t>(city.get_num_i()));
    for (int i = 0; i < city.get_num_i(); ++i) {
        const intersection& item = city.getIntersectionAt(i);
        mapView.intersections.push_back(
            {item.getid_i(), item.getname_i(), item.getx(), item.gety()});
    }

    mapView.roads.reserve(static_cast<size_t>(city.get_num_r()));
    for (int i = 0; i < city.get_num_r(); ++i) {
        const road& item = city.getRoadAt(i);
        mapView.roads.push_back(
            {item.getid_r(), item.getstart_id(), item.getend_id(),
             item.gettime(), item.getdistance()});
    }
}

bool RouteService::addIntersection(const IntersectionView& item, string& error) {
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
    return repository.save(city, error);
}

bool RouteService::updateIntersection(
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
    return repository.save(city, error);
}

bool RouteService::removeIntersection(int id, string& error) {
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
    return repository.save(city, error);
}

bool RouteService::addRoad(const RoadView& item, string& error) {
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
    return repository.save(city, error);
}

bool RouteService::updateRoad(int id, const RoadView& item, string& error) {
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
    return repository.save(city, error);
}

bool RouteService::removeRoad(int id, string& error) {
    if (city.find_r(id) == -1) {
        error = "Khong tim thay duong can xoa.";
        return false;
    }
    city.remove_r(id);
    createMapView();
    return repository.save(city, error);
}

RouteResult RouteService::findTaxiRoute(
    int taxiId, int pickupId, int destinationId, RouteMode mode) const {
    RouteResult result;
    if (!ready) {
        result.error = loadError.empty() ? "Du lieu ban do chua san sang." : loadError;
        return result;
    }

    const int intersectionCount = city.get_num_i();
    if (city.find_i(taxiId) == -1 || city.find_i(pickupId) == -1 ||
        city.find_i(destinationId) == -1) {
        result.error = "Giao lo duoc chon khong ton tai.";
        return result;
    }

    vector<int> firstPath(static_cast<size_t>(intersectionCount));
    vector<int> firstRoadIds(static_cast<size_t>(intersectionCount));
    vector<int> secondPath(static_cast<size_t>(intersectionCount));
    vector<int> secondRoadIds(static_cast<size_t>(intersectionCount));
    int firstPathCount = 0;
    int secondPathCount = 0;
    dijkstra algorithm;

    if (mode == RouteMode::Fastest) {
        algorithm.fastestPath(city, taxiId, pickupId, firstPath.data(),
                              firstPathCount, firstRoadIds.data(), false);
    } else {
        algorithm.shortestPath(city, taxiId, pickupId, firstPath.data(),
                               firstPathCount, firstRoadIds.data(), false);
    }
    if (firstPathCount == 0) {
        result.error = "Khong co duong tu taxi den diem don.";
        return result;
    }

    if (mode == RouteMode::Fastest) {
        algorithm.fastestPath(city, pickupId, destinationId, secondPath.data(),
                              secondPathCount, secondRoadIds.data(), false);
    } else {
        algorithm.shortestPath(city, pickupId, destinationId, secondPath.data(),
                               secondPathCount, secondRoadIds.data(), false);
    }
    if (secondPathCount == 0) {
        result.error = "Khong co duong tu diem don den dich.";
        return result;
    }

    result.intersectionIds.insert(
        result.intersectionIds.end(), firstPath.begin(), firstPath.begin() + firstPathCount);
    result.roadIds.insert(
        result.roadIds.end(), firstRoadIds.begin(), firstRoadIds.begin() + firstPathCount - 1);
    result.intersectionIds.insert(
        result.intersectionIds.end(), secondPath.begin() + 1,
        secondPath.begin() + secondPathCount);
    result.roadIds.insert(
        result.roadIds.end(), secondRoadIds.begin(),
        secondRoadIds.begin() + secondPathCount - 1);
    result.firstLegEdgeCount = static_cast<size_t>(firstPathCount - 1);

    for (size_t i = 0; i < result.roadIds.size(); ++i) {
        double distance = city.getRoadDistanceById(
            result.roadIds[i], result.intersectionIds[i], result.intersectionIds[i + 1]);
        double time = city.getRoadTimeById(
            result.roadIds[i], result.intersectionIds[i], result.intersectionIds[i + 1]);
        if (distance < 0.0 || time < 0.0) {
            result.intersectionIds.clear();
            result.roadIds.clear();
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
