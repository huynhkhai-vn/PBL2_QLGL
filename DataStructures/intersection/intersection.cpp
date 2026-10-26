#include <iostream>
#include "intersection.h"

using namespace std;

intersection::intersection(int id_i, string name_i, double x, double y) : 
    id_i(id_i),name_i(name_i),x(x),y(y){}



intersection::intersection(const intersection &other){
    id_i = other.id_i;
    name_i = other.name_i;
    x = other.x;
    y = other.y;
}

intersection::~intersection(){

}


int intersection::getid_i() const{
    return id_i;
}

string intersection::getname_i() const{
    return name_i;
}

double intersection::getx() const{
    return x;
}

double intersection::gety() const{
    return y;
}

void intersection::show() const{
    cout << id_i << " " << name_i << " " << x << " " << y << endl;
}
