#ifndef TEXT_MAP_REPOSITORY_H
#define TEXT_MAP_REPOSITORY_H

#include "BLL/map_repository.h"
#include <string>
#include <utility>

using namespace std;

class TextMapRepository : public IMapRepository {
private:
    string intersectionsFile;
    string roadsFile;

public:
    TextMapRepository(
        string intersectionsFile = "Data/intersections.txt",
        string roadsFile = "Data/roads.txt");

    bool load(map& target, string& error) const override;
};

TextMapRepository::TextMapRepository(
    string intersectionsFile, string roadsFile)
    : intersectionsFile(move(intersectionsFile)),
      roadsFile(move(roadsFile)) {}

bool TextMapRepository::load(map& target, string& error) const {
    error.clear();
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
}

#endif
