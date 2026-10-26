#ifndef DIJKSTRA_H
#define DIJKSTRA_H


#include "../../DataStructures/map/map.h"
#include "../../DataStructures/priorityqueue/minheap.h"

class dijkstra {
    private:
        void run(const map &m, int start_id, int end_id,
                double (road::*weight)() const, 
                int *path, int &count, int *road_ids);

    public:
        void shortestPath(const map &m, int start_id, int end_id,
                          int *path, int &count, int *road_ids = nullptr);
        void fastestPath(const map &m, int start_id, int end_id,
                         int *path, int &count, int *road_ids = nullptr);
};


#endif