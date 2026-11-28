// 00:16 AM ngày 21/9/2026

#ifndef MAP_H
#define MAP_H

#include "../intersection.h"
#include "../road.h"
#include "adjnode.h"
#include "linkedlist.h"
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>

using namespace std;

class dijkstra;

class map {
    friend class dijkstra;

    private:
        intersection* intersections;
        int num_i;
        int size_i;
        road* roads;
        int num_r;
        int size_r;
        linkedlist<AdjNode>* graph;

    public:
        map(int size_i = 100, int size_r = 100);
        ~map();

        bool add_i(intersection i);
        bool update_i(int id_i, const string& name, double x, double y);
        void remove_i(int id_i);

        bool add_r(road r);
        bool update_r(int id_r, int start_id, int end_id, double time, double distance);
        void remove_r(int id_r);

        int get_num_i() const;
        int get_size_i() const;
        int get_num_r() const;
        int get_size_r() const;

        const intersection& getIntersectionAt(int index) const;
        const road& getRoadAt(int index) const;
        double getRoadTime(int start_id, int end_id) const;
        double getRoadDistance(int start_id, int end_id) const;
        double getRoadTimeById(int id_r, int start_id, int end_id) const;
        double getRoadDistanceById(int id_r, int start_id, int end_id) const;

        int isEmpty_i() const;
        int isFull_i() const;
        int isEmpty_r() const;
        int isFull_r() const;
        int find_i(int id_i) const;
        int find_r(int id_r) const;

        void show_i() const;
        void show_r() const;
        void show_map() const;
    };


// Constructor map
map::map(int size_i, int size_r)
    : intersections(nullptr), num_i(0), size_i(size_i),
      roads(nullptr), num_r(0), size_r(size_r), graph(nullptr) {
    if (size_i < 0 || size_r < 0) {
        throw invalid_argument("Map capacities cannot be negative");
    }
    intersection* new_intersections = nullptr;
    road* new_roads = nullptr;
    linkedlist<AdjNode>* new_graph = nullptr;
    try {
        new_intersections = new intersection[size_i];
        new_roads = new road[size_r];
        new_graph = new linkedlist<AdjNode>[size_i];
    } catch (...) {
        delete[] new_intersections;
        delete[] new_roads;
        delete[] new_graph;
        throw;
    }

    intersections = new_intersections;
    roads = new_roads;
    graph = new_graph;
}

// Destructor map
map::~map() {
    delete[] intersections;
    delete[] roads;
    delete[] graph;
}

// Thêm giao lộ vào bản đồ
bool map::add_i(intersection i) {
    if (isFull_i() || find_i(i.getid_i()) != -1) {
        return false;
    }
    if (!isfinite(i.getx()) || !isfinite(i.gety())) {
        return false;
    }
    intersections[num_i] = i;
    num_i++;
    return true;
}

// Cập nhật thông tin giao lộ
bool map::update_i(int id_i, const string& name, double x, double y) {
    int index = find_i(id_i);
    if (index == -1 || !isfinite(x) || !isfinite(y)) {
        return false;
    }

    intersections[index] = intersection(id_i, name, x, y);
    return true;
}

// Xóa giao lộ khỏi bản đồ
void map::remove_i(int id_i) {
    int index = find_i(id_i);
    if (index == -1) {
        return;
    }
    for (int i = num_r - 1; i >= 0; i--) {
        if (roads[i].getstart_id() == id_i || roads[i].getend_id() == id_i) {
            remove_r(roads[i].getid_r());
        }
    }
    for (int i = index; i < num_i - 1; i++) {
        intersections[i] = intersections[i + 1];
    }
    num_i--;
    for (int i = index; i < num_i; i++) {
        graph[i] = graph[i + 1];
    }
    graph[num_i] = linkedlist<AdjNode>();
}

// Thêm đường vào bản đồ
bool map::add_r(road r) {
    if (isFull_r() || find_r(r.getid_r()) != -1) {
        return false;
    }
    if (!isfinite(r.gettime()) || !isfinite(r.getdistance()) ||
        r.gettime() < 0 || r.getdistance() < 0) {
        return false;
    }

    int start = find_i(r.getstart_id());
    int end = find_i(r.getend_id());
    if (start == -1 || end == -1 || start == end) {
        return false;
    }

    roads[num_r] = r;
    road edge = roads[num_r];
    graph[start].push(AdjNode(r.getend_id(), edge));
    graph[end].push(AdjNode(r.getstart_id(), edge));
    num_r++;
    return true;
}

// Cập nhật thông tin đường
bool map::update_r(int id_r, int start_id, int end_id, double time, double distance) {
    int index = find_r(id_r);
    int start = find_i(start_id);
    int end = find_i(end_id);
    if (index == -1 || start == -1 || end == -1 || start == end ||
        !isfinite(time) || !isfinite(distance) || time < 0 || distance < 0) {
        return false;
    }

    remove_r(id_r);
    return add_r(road(id_r, start_id, end_id, time, distance));
}

// Xóa đường khỏi bản đồ
void map::remove_r(int id_r) {
    int index = find_r(id_r);
    if (index == -1) {
        return;
    }
    int start = roads[index].getstart_id();
    int end = roads[index].getend_id();
    int start_index = find_i(start);
    int end_index = find_i(end);
    graph[start_index].remove_if([id_r](const AdjNode& node) {
        return node.get_r().getid_r() == id_r;
    });
    graph[end_index].remove_if([id_r](const AdjNode& node) {
        return node.get_r().getid_r() == id_r;
    });
    for (int i = index; i < num_r - 1; i++) {
        roads[i] = roads[i + 1];
    }
    num_r--;
}

// Getter
int map::get_num_i() const { return num_i; }
int map::get_size_i() const { return size_i; }
int map::get_num_r() const { return num_r; }
int map::get_size_r() const { return size_r; }

// Getter giao lộ
const intersection& map::getIntersectionAt(int index) const {
    if (index < 0 || index >= num_i) {
        throw out_of_range("Intersection index out of range");
    }
    return intersections[index];
}

// Getter đường
const road& map::getRoadAt(int index) const {
    if (index < 0 || index >= num_r) {
        throw out_of_range("Road index out of range");
    }
    return roads[index];
}

// Getter thời gian
double map::getRoadTime(int start_id, int end_id) const {
    double result = -1;
    for (int i = 0; i < num_r; i++) {
        if ((roads[i].getstart_id() == start_id && roads[i].getend_id() == end_id) ||
            (roads[i].getstart_id() == end_id && roads[i].getend_id() == start_id)) {
            if (result >= 0) {
                return -1;
            }
            result = roads[i].gettime();
        }
    }
    return result;
}

// Getter khoảng cách
double map::getRoadDistance(int start_id, int end_id) const {
    double result = -1;
    for (int i = 0; i < num_r; i++) {
        if ((roads[i].getstart_id() == start_id && roads[i].getend_id() == end_id) ||
            (roads[i].getstart_id() == end_id && roads[i].getend_id() == start_id)) {
            if (result >= 0) {
                return -1;
            }
            result = roads[i].getdistance();
        }
    }
    return result;
}

// Getter thời gian theo ID đường
double map::getRoadTimeById(int id_r, int start_id, int end_id) const {
    int index = find_r(id_r);
    if (index == -1 ||
        !((roads[index].getstart_id() == start_id && roads[index].getend_id() == end_id) ||
          (roads[index].getstart_id() == end_id && roads[index].getend_id() == start_id))) {
        return -1;
    }
    return roads[index].gettime();
}

// Getter khoảng cách theo ID đường
double map::getRoadDistanceById(int id_r, int start_id, int end_id) const {
    int index = find_r(id_r);
    if (index == -1 ||
        !((roads[index].getstart_id() == start_id && roads[index].getend_id() == end_id) ||
          (roads[index].getstart_id() == end_id && roads[index].getend_id() == start_id))) {
        return -1;
    }
    return roads[index].getdistance();
}

int map::isEmpty_i() const { return num_i == 0 ? 1 : 0; }
int map::isFull_i() const { return num_i == size_i ? 1 : 0; }
int map::isEmpty_r() const { return num_r == 0 ? 1 : 0; }
int map::isFull_r() const { return num_r == size_r ? 1 : 0; }

int map::find_i(int id_i) const {
    for (int i = 0; i < num_i; i++) {
        if (intersections[i].getid_i() == id_i) {
            return i;
        }
    }
    return -1;
}

int map::find_r(int id_r) const {
    for (int i = 0; i < num_r; i++) {
        if (roads[i].getid_r() == id_r) {
            return i;
        }
    }
    return -1;
}

void map::show_i() const {
    for (int i = 0; i < num_i; i++) {
        cout << intersections[i].getid_i() << " "
             << intersections[i].getname_i() << " "
             << intersections[i].getx() << " "
             << intersections[i].gety() << endl;
    }
}

void map::show_r() const {
    for (int i = 0; i < num_r; i++) {
        cout << roads[i].getid_r() << " "
             << roads[i].getstart_id() << " "
             << roads[i].getend_id() << " "
             << roads[i].gettime() << " "
             << roads[i].getdistance() << endl;
    }
}

void map::show_map() const {
    for (int i = 0; i < num_i; i++) {
        cout << "Intersection "
             << intersections[i].getid_i()
             << " - "
             << intersections[i].getname_i()
             << ":" << endl;

        auto* current = graph[i].get_head();
        while (current != nullptr) {
            cout << "    -> Intersection "
                 << current->get_data().get_i()
                 << " | Road "
                 << current->get_data().get_r().getid_r()
                 << " | Time: "
                 << current->get_data().get_r().gettime()
                 << " | Distance: "
                 << current->get_data().get_r().getdistance()
                 << endl;
            current = current->get_next();
        }
        cout << endl;
    }
}

#endif
