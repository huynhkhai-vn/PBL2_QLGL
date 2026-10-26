#ifndef TAXI_H
#define TAXI_H
#include "../../DataStructures/map/map.h"
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
    void move(const map &m);
};

#endif