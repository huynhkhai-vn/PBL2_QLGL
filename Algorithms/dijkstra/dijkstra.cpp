#include "dijkstra.h"
#include <iostream>
#include <cmath>
#include <limits>
#include <vector>

using namespace std;






void dijkstra::run(const map &m, int start_id, int end_id,
                   double (road::*weight)() const,
                   int *path, int &count, int *road_ids)
{
    count = 0;
    if(path == nullptr){
        cout << "Bo dem duong di khong hop le!" << endl;
        return;
    }

    int start = m.find_i(start_id);
    int end = m.find_i(end_id);

    if(start == -1 || end == -1) {
        cout << "Giao lo khong ton tai!" << endl;
        return;
    }

    int n = m.get_num_i();

    const double INF = std::numeric_limits<double>::infinity();

    vector<double> dist(n, INF);
    vector<int> parent(n, -1);
    vector<int> parent_road(n, -1);

    dist[start] = 0;
    minheap pq(2 * m.get_num_r() + 1);
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
        AdjNode* p = m.graph[u].get_head();

        while(p != nullptr) {

            int v = m.find_i(p->get_i());
            double w = (p->get_r().*weight)();

            if(!std::isfinite(w) || w < 0){
                cout << "Trong so duong di khong hop le!" << endl;
                return;
            }

            if(v != -1 && dist[u] + w < dist[v]) {
                dist[v] = dist[u] + w;
                parent[v] = u;
                parent_road[v] = p->get_r().getid_r();

                pq.push(hnode(p->get_i(), dist[v]));
            }


            p = p->get_next();
        }
    }

    if(dist[end] == INF) {
        cout << "Khong tim thay duong di!" << endl;

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

void dijkstra::shortestPath(const map &m, int start_id, int end_id,
                            int *path, int &count, int *road_ids) {
    run(m, start_id, end_id, &road::getdistance, path, count, road_ids);
}

void dijkstra::fastestPath(const map &m, int start_id, int end_id,
                           int *path, int &count, int *road_ids) {
    run(m, start_id, end_id, &road::gettime, path, count, road_ids);
}