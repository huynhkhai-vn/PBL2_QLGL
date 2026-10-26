#include "roadnode.h"




rnode::rnode()
    : edge(), next(nullptr), prev(nullptr){}

rnode::rnode(road r)
    : edge(r), next(nullptr), prev(nullptr){}



rnode::~rnode(){}


road &rnode::get_r() {
    return edge;
}

rnode* rnode::get_next() const {
    return next;
}

rnode* rnode::get_prev() const {
    return prev;
}


void rnode::set_next(rnode* next){
    this->next = next;
}

void rnode::set_prev(rnode* prev){
    this->prev = prev;
}