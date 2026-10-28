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

        intersection(int id_i = 0, string name_i = "", double x = 0, double y = 0)
            : id_i(id_i), name_i(name_i), x(x), y(y) {}
        intersection(const intersection &other) = default;
        ~intersection() = default;

        int getid_i() const { return id_i; }
        string getname_i() const { return name_i; }
        double getx() const { return x; }
        double gety() const { return y; }

        void show() const {
            cout << id_i << " " << name_i << " " << x << " " << y << endl;
        }


    
};


#endif