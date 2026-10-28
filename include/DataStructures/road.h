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
        
        road(int id_r = 0, int start_id = 0, int end_id = 0, double time = 0, double distance = 0)
            : id_r(id_r), start_id(start_id), end_id(end_id), time(time), distance(distance) {}
        road(const road &other) = default;
        ~road() = default;

        int getid_r() const { return id_r; }
        // string getname_r() const;
        int getstart_id() const { return start_id; }
        int getend_id() const { return end_id; }
        double gettime() const { return time; }
        double getdistance() const { return distance; }

        void show() const {
            cout << id_r << " " << start_id << " " << end_id << " "
                 << time << " " << distance << endl;
        }

};


#endif