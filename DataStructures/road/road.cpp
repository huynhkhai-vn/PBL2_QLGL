#include "road.h"
#include <iostream>


using namespace std;

road::road(int id_r, int start_id, int end_id, double time, double distance) :
    id_r(id_r),start_id(start_id),end_id(end_id),time(time),distance(distance){}


road::road(const road &other){
    id_r = other.id_r;
    start_id = other.start_id;
    end_id = other.end_id;
    time = other.time;
    distance = other.distance;
}

road::~road(){}


int road::getid_r() const {
    return id_r;
}

int road::getstart_id() const {
    return start_id;
}

int road::getend_id() const {
    return end_id;
}

double road::gettime() const {
    return time;
}

double road::getdistance() const {
    return distance;
}

void road::show() const{
    cout << id_r << " " << start_id << " " << end_id << " "
         << time << " " << distance << endl;
}