#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <cmath>
#include <cstddef>
#include <iostream>
#include <limits>
#include <vector>

#include "DataStructures/map/adjnode.h"
#include "DataStructures/map/map.h"
#include "DataStructures/road.h"
#include "DataStructures/priorityqueue/hnode.h"
#include "DataStructures/priorityqueue/minheap.h"

using namespace std;

struct hnode_cost_compare {
    bool operator()(const hnode& left, const hnode& right) const;
};

bool hnode_cost_compare::operator()(const hnode& left, const hnode& right) const {
    return left.get_cost() < right.get_cost();
}

class dijkstra {
    private:
        void run(const map &m, int start_id, int end_id,
                double (road::*weight)() const, 
                int *path, int &count, int *road_ids, bool print_result);
    public:
        void shortestPath(const map &m, int start_id, int end_id,
                          int *path, int &count, int *road_ids = nullptr,
                          bool print_result = true);
        void fastestPath(const map &m, int start_id, int end_id,
                         int *path, int &count, int *road_ids = nullptr,
                         bool print_result = true);
};

void dijkstra::run(const map &m, int start_id, int end_id,
                          double (road::*weight)() const,
                          int *path, int &count, int *road_ids,
                          bool print_result)
{
    count = 0;
    if(path == nullptr){
        if (print_result) {
            cout << "Bo dem duong di khong hop le!" << endl;
        }
        return;
    }

    int start = m.find_i(start_id);
    int end = m.find_i(end_id);

    if(start == -1 || end == -1) {
        if (print_result) {
            cout << "Giao lo khong ton tai!" << endl;
        }
        return;
    }

    int n = m.get_num_i();

    const double INF = numeric_limits<double>::infinity();

    vector<double> dist(n, INF);
    vector<int> parent(n, -1);
    vector<int> parent_road(n, -1);

    dist[start] = 0;
    minheap<hnode, hnode_cost_compare> pq(2 * m.get_num_r() + 1);
    pq.push(hnode(start_id, 0));

    while(!pq.isEmpty()) {
        hnode current = pq.pop();
        int u = m.find_i(current.getid_i());

        if(u == -1)
            continue;
        if(current.get_cost() > dist[u]) {
            continue;
        }
        if(u == end) {
            break;
        }
        auto* p = m.graph[u].get_head();

        while(p != nullptr) {

            const AdjNode& adjacent = p->get_data();
            int v = m.find_i(adjacent.get_i());
            double w = (adjacent.get_r().*weight)();

            if(!isfinite(w) || w < 0){
                if (print_result) {
                    cout << "Trong so duong di khong hop le!" << endl;
                }
                return;
            }

            if(v != -1 && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                parent_road[v] = adjacent.get_r().getid_r();

                pq.push(hnode(adjacent.get_i(), dist[v]));
            }


            p = p->get_next();
        }
    }

    if(dist[end] == INF) {
        if (print_result) {
            cout << "Khong tim thay duong di!" << endl;
        }

        return;
    }

    int current = end;
    vector<int> reversed_roads;

    while(current != -1) {
        path[count] = m.intersections[current].getid_i();
        if(parent_road[current] != -1) {
            reversed_roads.push_back(parent_road[current]);
        }
        count++;

        current = parent[current];
    }

    for(int i = 0; i < count / 2; i++) {
        int tmp = path[i];
        path[i] = path[count - 1 - i];
        path[count - 1 - i] = tmp;
    }
    if(road_ids != nullptr) {
        for(size_t i = 0; i < reversed_roads.size(); i++) {
            road_ids[i] = reversed_roads[reversed_roads.size() - 1 - i];
        }
    }

    if (print_result) {
        if(weight == &road::getdistance)
            cout << "Duong di ngan nhat: ";
        else
            cout << "Duong di nhanh nhat: ";

        for(int i = 0; i < count; i++) {
            cout << path[i];

            if(i < count - 1)
                cout << " -> ";
        }

        cout << endl;

        if(weight == &road::getdistance)
            cout << "Tong khoang cach: ";
        else
            cout << "Tong thoi gian: ";

        cout << dist[end] << endl;
    }

}

void dijkstra::shortestPath(const map &m, int start_id, int end_id,
                                   int *path, int &count, int *road_ids,
                                   bool print_result) {
    run(m, start_id, end_id, &road::getdistance, path, count, road_ids, print_result);
}

void dijkstra::fastestPath(const map &m, int start_id, int end_id,
                                  int *path, int &count, int *road_ids,
                                  bool print_result) {
    run(m, start_id, end_id, &road::gettime, path, count, road_ids, print_result);
}


#endif