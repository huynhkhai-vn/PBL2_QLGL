#include "map.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cmath>
#include <memory>
#include <stdexcept>
#include <vector>

using namespace std;

namespace {
template <typename T>
bool parse_value(const string& text, T& value) {
    istringstream stream(text);
    stream >> value;
    if(stream.fail()) {
        return false;
    }
    stream >> ws;
    return stream.eof();
}

bool split_fields(const string& line, size_t expected, vector<string>& fields) {
    fields.clear();
    string field;
    stringstream stream(line);
    while(getline(stream, field, '|')) {
        fields.push_back(field);
    }
    return fields.size() == expected;
}
}

map::map(int size_i, int size_r)
    : intersections(nullptr), num_i(0), size_i(size_i),
      roads(nullptr), num_r(0), size_r(size_r), graph(nullptr) {
    if(size_i < 0 || size_r < 0) {
        throw invalid_argument("Map capacities cannot be negative");
    }

    unique_ptr<intersection[]> new_intersections(new intersection[size_i]);
    unique_ptr<road[]> new_roads(new road[size_r]);
    unique_ptr<linkedlist[]> new_graph(new linkedlist[size_i]);

    intersections = new_intersections.release();
    roads = new_roads.release();
    graph = new_graph.release();
}

map::~map(){
    delete[] intersections;
    delete[] roads;
    delete[] graph;

}


int map::get_num_i() const {
    return num_i;
}

int map::get_size_i() const {
    return size_i;
}

int map::get_num_r() const {
    return num_r;
}

int map::get_size_r() const {
    return size_r;
}

const intersection& map::getIntersectionAt(int index) const {
    if(index < 0 || index >= num_i) {
        throw out_of_range("Intersection index out of range");
    }
    return intersections[index];
}

const road& map::getRoadAt(int index) const {
    if(index < 0 || index >= num_r) {
        throw out_of_range("Road index out of range");
    }
    return roads[index];
}

//Các hàm find trả về chỉ số index
int map::find_i(int id_i) const {
    for(int i = 0; i < num_i; i++){
        if(intersections[i].getid_i() == id_i){
            return i;
        }
    }

    return -1;
}


int map::find_r(int id_r) const {
    for(int i = 0; i < num_r; i++){
        if(roads[i].getid_r() == id_r){
            return i;
        }
    }

    return -1;
}

int map::isEmpty_i() const {
    if(num_i == 0)
        return 1;
    return 0;
}

int map::isFull_i() const {
    if(num_i == size_i)
        return 1;
    return 0;
}

int map::isEmpty_r() const {
    if(num_r == 0)
        return 1;
    return 0;
}

int map::isFull_r() const {
    if(num_r == size_r)
        return 1;
    return 0;
}




bool map::add_i(intersection i) {
    if(isFull_i())
        return false;

    if(find_i(i.getid_i()) != -1)
        return false;
    if(!std::isfinite(i.getx()) || !std::isfinite(i.gety()))
        return false;

    intersections[num_i] = i;
    num_i++;
    return true;
}


bool map::add_r(road r) {
    if(isFull_r())
        return false;

    if(find_r(r.getid_r()) != -1)
        return false;
    if(!std::isfinite(r.gettime()) || !std::isfinite(r.getdistance()) ||
       r.gettime() < 0 || r.getdistance() < 0)
        return false;

    int start = find_i(r.getstart_id());        //Gắn start, end = index 2 đỉnh đầu mút
    int end = find_i(r.getend_id());

    if(start == -1 || end == -1)
        return false;

    roads[num_r] = r;

    road edge = roads[num_r];

    graph[start].push(r.getend_id(), edge);
    graph[end].push(r.getstart_id(), edge);

    num_r++;
    return true;
}

void map::remove_r(int id_r) {
    int index = find_r(id_r);

    if(index == -1)
        return;

    int start = roads[index].getstart_id();
    int end = roads[index].getend_id();

    int start_index = find_i(start);
    int end_index = find_i(end);

    graph[start_index].remove(id_r);
    graph[end_index].remove(id_r);

    for(int i = index; i < num_r - 1; i++){
        roads[i] = roads[i + 1];
    }

    num_r--;
}



void map::remove_i(int id_i) {

    int index = find_i(id_i);
    if(index == -1)
        return;

    for(int i = num_r - 1; i >= 0; i--){
        if(roads[i].getstart_id() == id_i || roads[i].getend_id() == id_i){
                remove_r(roads[i].getid_r());
            }
        }
    for(int i = index; i < num_i - 1; i++){
        intersections[i] = intersections[i + 1];
    }

    num_i--;
    for(int i = index; i < num_i; i++){
        graph[i] = graph[i + 1];
    }

    graph[num_i] = linkedlist();

}



void map::show_i() const {
    for(int i = 0; i < num_i; i++){
        cout << intersections[i].getid_i() << " "
             << intersections[i].getname_i() << " "
             << intersections[i].getx() << " "
             << intersections[i].gety() << endl;
    }
}



void map::show_r() const {
    for(int i = 0; i < num_r; i++){
        cout << roads[i].getid_r() << " "
             << roads[i].getstart_id() << " "
             << roads[i].getend_id() << " "
             << roads[i].gettime() << " "
             << roads[i].getdistance() << endl;
    }
}


void map::show_map() const {
    for(int i = 0; i < num_i; i++){
        cout << "Intersection " 
             << intersections[i].getid_i()
             << " - "
             << intersections[i].getname_i()
             << ":" << endl;

        AdjNode* current = graph[i].get_head();

        while(current != nullptr){
            cout << "    -> Intersection "
                 << current->get_i()
                 << " | Road "
                 << current->get_r().getid_r()
                 << " | Time: "
                 << current->get_r().gettime()
                 << " | Distance: "
                 << current->get_r().getdistance()
                 << endl;

            current = current->get_next();
        }

        cout << endl;
    }
}


bool map::load_i(const string &filename){

    ifstream fin(filename);

    if(!fin){
        cerr << "Khong mo duoc file giao lo: " << filename << endl;
        return false;
    }
    
    string line;
    size_t line_number = 0;

    while(getline(fin,line)){
        ++line_number;
        if(line.empty()) {
            continue;
        }
        vector<string> fields;
        int id;
        double x, y;
        if(!split_fields(line, 4, fields) ||
           !parse_value(fields[0], id) ||
           !parse_value(fields[2], x) ||
           !parse_value(fields[3], y) ||
           !std::isfinite(x) || !std::isfinite(y) ||
           !add_i(intersection(id, fields[1], x, y))) {
            cerr << "Du lieu giao lo khong hop le tai "
                 << filename << ":" << line_number << endl;
            return false;
        }
    }
    if(fin.bad()) {
        cerr << "Loi khi doc file giao lo: " << filename << endl;
        return false;
    }
    return true;
}


bool map::load_r(const string &filename){
    ifstream fin(filename);

    if(!fin){
        cerr << "Khong mo duoc file duong: " << filename << endl;
        return false;
    }

    string line;
    size_t line_number = 0;

    while(getline(fin, line)){
        ++line_number;
        if(line.empty()) {
            continue;
        }
        vector<string> fields;
        int id, start, end;
        double time, distance;
        if(!split_fields(line, 5, fields) ||
           !parse_value(fields[0], id) ||
           !parse_value(fields[1], start) ||
           !parse_value(fields[2], end) ||
           !parse_value(fields[3], time) ||
           !parse_value(fields[4], distance) ||
           !std::isfinite(time) || !std::isfinite(distance) ||
           !add_r(road(id, start, end, time, distance))) {
            cerr << "Du lieu duong khong hop le tai "
                 << filename << ":" << line_number << endl;
            return false;
        }
    }

    if(fin.bad()) {
        cerr << "Loi khi doc file duong: " << filename << endl;
        return false;
    }
    return true;
}

double map::getRoadTime(int start_id, int end_id) const {
    double result = -1;
    for(int i = 0; i < num_r; i++) {
        if((roads[i].getstart_id() == start_id &&
            roads[i].getend_id() == end_id) ||
           (roads[i].getstart_id() == end_id &&
            roads[i].getend_id() == start_id)) {
            if(result >= 0) {
                return -1;
            }
            result = roads[i].gettime();
        }
    }

    return result;
}

double map::getRoadDistance(int start_id, int end_id) const {
    double result = -1;
    for(int i = 0; i < num_r; i++) {
        if((roads[i].getstart_id() == start_id &&
            roads[i].getend_id() == end_id) ||
           (roads[i].getstart_id() == end_id &&
            roads[i].getend_id() == start_id)) {
            if(result >= 0) {
                return -1;
            }
            result = roads[i].getdistance();
        }
    }

    return result;
}

double map::getRoadTimeById(int id_r, int start_id, int end_id) const {
    int index = find_r(id_r);
    if(index == -1 ||
       !((roads[index].getstart_id() == start_id &&
          roads[index].getend_id() == end_id) ||
         (roads[index].getstart_id() == end_id &&
          roads[index].getend_id() == start_id))) {
        return -1;
    }
    return roads[index].gettime();
}

double map::getRoadDistanceById(int id_r, int start_id, int end_id) const {
    int index = find_r(id_r);
    if(index == -1 ||
       !((roads[index].getstart_id() == start_id &&
          roads[index].getend_id() == end_id) ||
         (roads[index].getstart_id() == end_id &&
          roads[index].getend_id() == start_id))) {
        return -1;
    }
    return roads[index].getdistance();
}









