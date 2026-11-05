// 00:16 AM ngày 21/9/2026

#ifndef MAP_H
#define MAP_H

#include "../intersection.h"
#include "../road.h"
#include "adjnode.h"
#include "linkedlist.h"
#include <cmath>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

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

    map(const map&) = delete;
    map& operator=(const map&) = delete;

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

    bool load_i(const string& filename);
    bool load_r(const string& filename);
    void show_i() const;
    void show_r() const;
    void show_map() const;
};

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

map::~map() {
    delete[] intersections;
    delete[] roads;
    delete[] graph;
}

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

bool map::update_i(int id_i, const string& name, double x, double y) {
    int index = find_i(id_i);
    if (index == -1 || !isfinite(x) || !isfinite(y)) {
        return false;
    }

    intersections[index] = intersection(id_i, name, x, y);
    return true;
}

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

int map::get_num_i() const { return num_i; }
int map::get_size_i() const { return size_i; }
int map::get_num_r() const { return num_r; }
int map::get_size_r() const { return size_r; }

const intersection& map::getIntersectionAt(int index) const {
    if (index < 0 || index >= num_i) {
        throw out_of_range("Intersection index out of range");
    }
    return intersections[index];
}

const road& map::getRoadAt(int index) const {
    if (index < 0 || index >= num_r) {
        throw out_of_range("Road index out of range");
    }
    return roads[index];
}

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

double map::getRoadTimeById(int id_r, int start_id, int end_id) const {
    int index = find_r(id_r);
    if (index == -1 ||
        !((roads[index].getstart_id() == start_id && roads[index].getend_id() == end_id) ||
          (roads[index].getstart_id() == end_id && roads[index].getend_id() == start_id))) {
        return -1;
    }
    return roads[index].gettime();
}

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

bool map::load_i(const string& filename) {
    ifstream fin(filename);
    if (!fin) {
        throw runtime_error("Khong mo duoc file giao lo: " + filename);
    }

    string line;
    size_t lineNumber = 0;
    while (getline(fin, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }
        vector<string> fields;
        stringstream stream(line);
        string field;
        while (getline(stream, field, '|')) {
            fields.push_back(field);
        }
        if (fields.size() != 4) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": giao lo can co 4 truong.");
        }

        try {
            size_t idLength = 0, xLength = 0, yLength = 0;
            int id = stoi(fields[0], &idLength);
            double x = stod(fields[2], &xLength);
            double y = stod(fields[3], &yLength);
            if (idLength != fields[0].size() || xLength != fields[2].size() ||
                yLength != fields[3].size()) {
                throw invalid_argument("Du lieu so chua ky tu khong hop le.");
            }
            if (!add_i(intersection(id, fields[1], x, y))) {
                throw runtime_error(filename + ":" + to_string(lineNumber) +
                                    ": ID giao lo bi trung, toa do khong hop le "
                                    "hoac vuot suc chua ban do.");
            }
        } catch (const invalid_argument& error) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": " + error.what());
        } catch (const out_of_range& error) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": " + error.what());
        }
    }
    if (fin.bad()) {
        throw runtime_error("Loi khi doc file giao lo: " + filename);
    }
    return true;
}

bool map::load_r(const string& filename) {
    ifstream fin(filename);
    if (!fin) {
        throw runtime_error("Khong mo duoc file duong: " + filename);
    }

    string line;
    size_t lineNumber = 0;
    while (getline(fin, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }
        vector<string> fields;
        stringstream stream(line);
        string field;
        while (getline(stream, field, '|')) {
            fields.push_back(field);
        }
        if (fields.size() != 5) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": duong can co 5 truong.");
        }

        try {
            size_t idLength = 0, startLength = 0, endLength = 0;
            size_t timeLength = 0, distanceLength = 0;
            int id = stoi(fields[0], &idLength);
            int startId = stoi(fields[1], &startLength);
            int endId = stoi(fields[2], &endLength);
            double time = stod(fields[3], &timeLength);
            double distance = stod(fields[4], &distanceLength);
            if (idLength != fields[0].size() || startLength != fields[1].size() ||
                endLength != fields[2].size() || timeLength != fields[3].size() ||
                distanceLength != fields[4].size()) {
                throw invalid_argument("Du lieu so chua ky tu khong hop le.");
            }
            if (!add_r(road(id, startId, endId, time, distance))) {
                throw runtime_error(filename + ":" + to_string(lineNumber) +
                                    ": ID duong bi trung, giao lo khong ton tai, "
                                    "trong so khong hop le hoac vuot suc chua ban do.");
            }
        } catch (const invalid_argument& error) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": " + error.what());
        } catch (const out_of_range& error) {
            throw runtime_error(filename + ":" + to_string(lineNumber) +
                                ": " + error.what());
        }
    }
    if (fin.bad()) {
        throw runtime_error("Loi khi doc file duong: " + filename);
    }
    return true;
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
