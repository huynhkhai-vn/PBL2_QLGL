#include "anode.h"



anode::anode(int id_i)
    : id_i(id_i), adjlist(), next(nullptr), prev(nullptr){}




anode::~anode(){}

int anode::get_id() const {
    return id_i;
}

linkedlist& anode::get_list() {
    return adjlist;
}

anode* anode::get_next() const {
    return next;
}

anode* anode::get_prev() const {
    return prev;
}

void anode::set_next(anode* next){
    this->next = next;
}

void anode::set_prev(anode* prev) {
    this->prev = prev;
}