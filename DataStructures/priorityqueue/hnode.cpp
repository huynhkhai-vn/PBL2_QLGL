#include "hnode.h"

hnode::hnode()
    : id_i(0), cost(0){}

hnode::hnode(int id_i, double cost)
    : id_i(id_i), cost(cost){}

hnode::hnode(const hnode& other){
    id_i = other.id_i;
    cost = other.cost;
}

hnode::~hnode(){

}

int hnode::getid_i() const {
    return id_i;
}

double hnode::get_cost() const {
    return cost;
}