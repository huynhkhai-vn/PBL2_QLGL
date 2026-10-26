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
        ~road();

        int getid_r() const;
        // string getname_r() const;
        int getstart_id() const;
        int getend_id() const;
        double gettime() const;
        double getdistance() const;

        void show() const;

};


#endif