#ifndef TAXI_H
#define TAXI_H

#include "DataStructures/map/map.h"
#include <iostream>
#include <memory>

using namespace std;

class taxi {
private:
    int id_taxi;
    int current_id;
    int status;
    double total_time;
    double total_distance;
    int* path;
    int* road_ids;
    int path_size;
    int path_index;

public:
    taxi(int id, int start_id);
    ~taxi();
    taxi(const taxi&) = delete;
    taxi& operator=(const taxi&) = delete;

    int getid_taxi() const;
    int getcurrent_id() const;
    int getstatus() const;
    double gettotal_time() const;
    double gettotal_distance() const;

    void setPath(const int* new_path, int count);
    void setPath(const int* new_path, int count, const int* new_road_ids);
    void move(const map& m);
};

taxi::taxi(int id, int start_id)
    : id_taxi(id), current_id(start_id), status(0), total_time(0),
      total_distance(0), path(nullptr), road_ids(nullptr), path_size(0),
      path_index(0) {}

taxi::~taxi() {
    delete[] path;
    delete[] road_ids;
}

int taxi::getid_taxi() const { return id_taxi; }
int taxi::getcurrent_id() const { return current_id; }
int taxi::getstatus() const { return status; }
double taxi::gettotal_time() const { return total_time; }
double taxi::gettotal_distance() const { return total_distance; }

void taxi::setPath(const int* new_path, int count) {
    setPath(new_path, count, nullptr);
}

void taxi::setPath(const int* new_path, int count, const int* new_road_ids) {
    if (count <= 0 || new_path == nullptr) {
        if (count > 0) {
            cerr << "Khong the cai dat lo trinh rong!" << endl;
        }
        return;
    }

    unique_ptr<int[]> new_path_copy(new int[count]);
    unique_ptr<int[]> road_copy(count > 1 ? new int[count - 1] : nullptr);
    for (int i = 0; i < count; i++) {
        new_path_copy[i] = new_path[i];
    }
    for (int i = 0; i < count - 1; i++) {
        road_copy[i] = new_road_ids == nullptr ? -1 : new_road_ids[i];
    }

    delete[] path;
    delete[] road_ids;
    path = new_path_copy.release();
    road_ids = road_copy.release();
    path_size = count;
    path_index = 0;
    status = (count > 1) ? 1 : 0;
}

void taxi::move(const map& m) {
    if (status == 0 || path_index == path_size - 1) {
        return;
    }

    int next_id = path[path_index + 1];
    double t = road_ids[path_index] == -1
        ? m.getRoadTime(current_id, next_id)
        : m.getRoadTimeById(road_ids[path_index], current_id, next_id);
    double d = road_ids[path_index] == -1
        ? m.getRoadDistance(current_id, next_id)
        : m.getRoadDistanceById(road_ids[path_index], current_id, next_id);
    if (t < 0 || d < 0) {
        cerr << "Khong tim thay duong noi hai giao lo trong lo trinh taxi!"
             << endl;
        status = 0;
        return;
    }
    total_time += t;
    total_distance += d;
    path_index++;
    current_id = path[path_index];
    if (path_index == path_size - 1) {
        status = 0;
    }
}

#endif
