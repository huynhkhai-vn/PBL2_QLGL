#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <iostream>
#include <string>

using namespace std;

class intersection {
    private:
        int id_i;
        string name_i;
        double x;
        double y;
    public:

        intersection(int id_i = 0, string name_i = "", double x = 0, double y = 0);
        intersection(const intersection &other);
        intersection& operator=(const intersection& other);
        ~intersection();

        int getid_i() const;
        string getname_i() const;
        double getx() const;
        double gety() const;

        void show() const;

    
};

intersection::intersection(int id_i, string name_i, double x, double y)
    : id_i(id_i), name_i(name_i), x(x), y(y) {}

intersection::intersection(const intersection &other) = default;
intersection& intersection::operator=(const intersection& other) = default;
intersection::~intersection() = default;

int intersection::getid_i() const { return id_i; }
string intersection::getname_i() const { return name_i; }
double intersection::getx() const { return x; }
double intersection::gety() const { return y; }

void intersection::show() const {
    cout << id_i << " " << name_i << " " << x << " " << y << endl;
}

#endif