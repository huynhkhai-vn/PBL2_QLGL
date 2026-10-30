#ifndef ROAD_H
#define ROAD_H
#include <iostream>

using namespace std;

class road {
    private:
        int id_r;
        int start_id;
        int end_id;
        double time;
        double distance;
    public:
        
        road(int id_r = 0, int start_id = 0, int end_id = 0, double time = 0, double distance = 0);
        road(const road &other);
        road& operator=(const road& other);
        ~road();

        int getid_r() const;
        // string getname_r() const;
        int getstart_id() const;
        int getend_id() const;
        double gettime() const;
        double getdistance() const;

        void show() const;

};

road::road(int id_r, int start_id, int end_id, double time, double distance)
    : id_r(id_r), start_id(start_id), end_id(end_id), time(time), distance(distance) {}

road::road(const road &other) = default;
road& road::operator=(const road& other) = default;
road::~road() = default;

int road::getid_r() const { return id_r; }
int road::getstart_id() const { return start_id; }
int road::getend_id() const { return end_id; }
double road::gettime() const { return time; }
double road::getdistance() const { return distance; }

void road::show() const {
    cout << id_r << " " << start_id << " " << end_id << " "
         << time << " " << distance << endl;
}

#endif