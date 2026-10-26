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
        ~intersection();

        int getid_i() const;
        string getname_i() const;
        double getx() const;
        double gety() const;

        void show() const;


    
};


#endif