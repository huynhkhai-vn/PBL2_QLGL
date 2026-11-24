#ifndef MAP_DATA_ACCESS_H
#define MAP_DATA_ACCESS_H

#include "DataStructures/map/map.h"
#include <fstream>
#include <iomanip>
#include <string>
#include <utility>

using namespace std;

class MapDataAccess {
private:
    string intersectionsFile;
    string roadsFile;

public:
    MapDataAccess(
        string intersectionsFile = "Data/intersections.txt",
        string roadsFile = "Data/roads.txt");

    bool load(map& target, string& error) const;
    bool save(const map& source, string& error) const;
};

MapDataAccess::MapDataAccess(
    string intersectionsFile, string roadsFile)
    : intersectionsFile(move(intersectionsFile)),
      roadsFile(move(roadsFile)) {}

bool MapDataAccess::load(map& target, string& error) const {
    error.clear();
    try {
        if (!target.load_i(intersectionsFile)) {
            error = "Khong the nap giao lo tu file: " + intersectionsFile;
            return false;
        }
        if (!target.load_r(roadsFile)) {
            error = "Khong the nap duong tu file: " + roadsFile;
            return false;
        }
        if (target.get_num_i() == 0) {
            error = "Du lieu ban do khong co giao lo.";
            return false;
        }
        return true;
    } catch (const exception& exception) {
        error = exception.what();
        return false;
    }
}

bool MapDataAccess::save(const map& source, string& error) const {
    error.clear();

    ofstream intersectionsOut(intersectionsFile, ios::trunc);
    if (!intersectionsOut) {
        error = "Khong the ghi file giao lo: " + intersectionsFile;
        return false;
    }
    intersectionsOut << setprecision(15);
    for (int i = 0; i < source.get_num_i(); ++i) {
        const intersection& item = source.getIntersectionAt(i);
        intersectionsOut << item.getid_i() << '|'
                         << item.getname_i() << '|'
                         << item.getx() << '|'
                         << item.gety() << '\n';
    }
    intersectionsOut.close();
    if (!intersectionsOut) {
        error = "Loi khi ghi file giao lo: " + intersectionsFile;
        return false;
    }

    ofstream roadsOut(roadsFile, ios::trunc);
    if (!roadsOut) {
        error = "Khong the ghi file duong: " + roadsFile;
        return false;
    }
    roadsOut << setprecision(15);
    for (int i = 0; i < source.get_num_r(); ++i) {
        const road& item = source.getRoadAt(i);
        roadsOut << item.getid_r() << '|'
                 << item.getstart_id() << '|'
                 << item.getend_id() << '|'
                 << item.gettime() << '|'
                 << item.getdistance() << '\n';
    }
    roadsOut.close();
    if (!roadsOut) {
        error = "Loi khi ghi file duong: " + roadsFile;
        return false;
    }
    return true;
}

#endif
