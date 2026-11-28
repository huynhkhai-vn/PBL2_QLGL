#ifndef MAP_DATA_ACCESS_H
#define MAP_DATA_ACCESS_H

#include "DataStructures/map/map.h"
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <utility>

using namespace std;


// Khái báo nguyên mẫu hàm
class MapDataAccess {
private:
    string intersectionsFile;
    string roadsFile;

    void loadIntersections(map& target) const;
    void loadRoads(map& target) const;

public:
    MapDataAccess(
        string intersectionsFile = "Data/intersections.txt",
        string roadsFile = "Data/roads.txt");

    bool load(map& target, string& error) const;
    bool save(const map& source, string& error) const;
};




// Định nghĩa hàm
MapDataAccess::MapDataAccess(
    string intersectionsFile, string roadsFile)
    : intersectionsFile(move(intersectionsFile)),
      roadsFile(move(roadsFile)) {}

bool MapDataAccess::load(map& target, string& error) const {
    error.clear();
    try {
        loadIntersections(target);
        loadRoads(target);
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

void MapDataAccess::loadIntersections(map& target) const {
    ifstream fin(intersectionsFile);
    if (!fin) {
        throw runtime_error("Khong mo duoc file giao lo: " + intersectionsFile);
    }

    string line;
    int lineNumber = 0;
    while (getline(fin, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }
        string fields[4];
        int fieldCount = 0;
        size_t fieldStart = 0;
        while (true) {
            if (fieldCount == 4) {
                throw runtime_error(intersectionsFile + ":" + to_string(lineNumber) +
                                    ": giao lo can co 4 truong.");
            }
            size_t delimiter = line.find('|', fieldStart);
            fields[fieldCount++] = line.substr(
                fieldStart, delimiter == string::npos ? string::npos : delimiter - fieldStart);
            if (delimiter == string::npos) {
                break;
            }
            fieldStart = delimiter + 1;
        }
        if (fieldCount != 4) {
            throw runtime_error(intersectionsFile + ":" + to_string(lineNumber) +
                                ": giao lo can co 4 truong.");
        }

        try {
            size_t idLength = 0, xLength = 0, yLength = 0;
            int id = stoi(fields[0], &idLength);
            double x = stod(fields[2], &xLength);
            double y = stod(fields[3], &yLength);
            if (idLength != fields[0].size() || xLength != fields[2].size() ||
                yLength != fields[3].size()) {
                throw invalid_argument("Du lieu so chua ky tu khong hop le.");
            }
            if (!target.add_i(intersection(id, fields[1], x, y))) {
                throw runtime_error(intersectionsFile + ":" + to_string(lineNumber) +
                                    ": ID giao lo bi trung, toa do khong hop le "
                                    "hoac vuot suc chua ban do.");
            }
        } catch (const invalid_argument& error) {
            throw runtime_error(intersectionsFile + ":" + to_string(lineNumber) +
                                ": " + error.what());
        } catch (const out_of_range& error) {
            throw runtime_error(intersectionsFile + ":" + to_string(lineNumber) +
                                ": " + error.what());
        }
    }
    if (fin.bad()) {
        throw runtime_error("Loi khi doc file giao lo: " + intersectionsFile);
    }
}

void MapDataAccess::loadRoads(map& target) const {
    ifstream fin(roadsFile);
    if (!fin) {
        throw runtime_error("Khong mo duoc file duong: " + roadsFile);
    }

    string line;
    int lineNumber = 0;
    while (getline(fin, line)) {
        ++lineNumber;
        if (line.empty()) {
            continue;
        }
        string fields[5];
        int fieldCount = 0;
        size_t fieldStart = 0;
        while (true) {
            if (fieldCount == 5) {
                throw runtime_error(roadsFile + ":" + to_string(lineNumber) +
                                    ": duong can co 5 truong.");
            }
            size_t delimiter = line.find('|', fieldStart);
            fields[fieldCount++] = line.substr(
                fieldStart, delimiter == string::npos ? string::npos : delimiter - fieldStart);
            if (delimiter == string::npos) {
                break;
            }
            fieldStart = delimiter + 1;
        }
        if (fieldCount != 5) {
            throw runtime_error(roadsFile + ":" + to_string(lineNumber) +
                                ": duong can co 5 truong.");
        }

        try {
            size_t idLength = 0, startLength = 0, endLength = 0;
            size_t timeLength = 0, distanceLength = 0;
            int id = stoi(fields[0], &idLength);
            int startId = stoi(fields[1], &startLength);
            int endId = stoi(fields[2], &endLength);
            double time = stod(fields[3], &timeLength);
            double distance = stod(fields[4], &distanceLength);
            if (idLength != fields[0].size() || startLength != fields[1].size() ||
                endLength != fields[2].size() || timeLength != fields[3].size() ||
                distanceLength != fields[4].size()) {
                throw invalid_argument("Du lieu so chua ky tu khong hop le.");
            }
            if (!target.add_r(road(id, startId, endId, time, distance))) {
                throw runtime_error(roadsFile + ":" + to_string(lineNumber) +
                                    ": ID duong bi trung, giao lo khong ton tai, "
                                    "trong so khong hop le hoac vuot suc chua ban do.");
            }
        } catch (const invalid_argument& error) {
            throw runtime_error(roadsFile + ":" + to_string(lineNumber) +
                                ": " + error.what());
        } catch (const out_of_range& error) {
            throw runtime_error(roadsFile + ":" + to_string(lineNumber) +
                                ": " + error.what());
        }
    }
    if (fin.bad()) {
        throw runtime_error("Loi khi doc file duong: " + roadsFile);
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
