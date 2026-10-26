#include "intersectionnode.h"


inode::inode()
    : vertex(), next(nullptr), prev(nullptr){}

inode::inode(intersection i)
    : vertex(i), next(nullptr), prev(nullptr){}

inode::~inode(){

}

intersection inode::get_i() const{
    return vertex;
}

inode* inode::get_next() const{
    return next;
}

inode* inode::get_prev() const {
    return prev;
}

void inode::set_next(inode* next){
    this->next = next;
}

void inode::set_prev(inode* prev){
    this->prev = prev;
}