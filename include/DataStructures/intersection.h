#ifndef INTERSECTION_H
#define INTERSECTION_H

#include <iostream>
#include <string>

class intersection {
    private:
        int id_i;
        std::string name_i;
        double x;
        double y;
    public:

        intersection(int id_i = 0, std::string name_i = "", double x = 0, double y = 0);
        intersection(const intersection &other);
        intersection& operator=(const intersection& other);
        ~intersection();

        int getid_i() const;
        std::string getname_i() const;
        double getx() const;
        double gety() const;

        void show() const;

    
};

inline intersection::intersection(int id_i, std::string name_i, double x, double y)
    : id_i(id_i), name_i(name_i), x(x), y(y) {}

inline intersection::intersection(const intersection &other) = default;
inline intersection& intersection::operator=(const intersection& other) = default;
inline intersection::~intersection() = default;

inline int intersection::getid_i() const { return id_i; }
inline std::string intersection::getname_i() const { return name_i; }
inline double intersection::getx() const { return x; }
inline double intersection::gety() const { return y; }

inline void intersection::show() const {
    std::cout << id_i << " " << name_i << " " << x << " " << y << std::endl;
}

#endif