#ifndef MAP_REPOSITORY_H
#define MAP_REPOSITORY_H

#include "DataStructures/map/map.h"
#include <string>

using namespace std;

class IMapRepository {
public:
    virtual ~IMapRepository() = default;
    virtual bool load(map& target, string& error) const = 0;
};

#endif
