#include "intersectionlist.h"


ilist::ilist()
    : head(nullptr), tail(nullptr), size(0){}

ilist::~ilist(){
    while(head != nullptr){
        inode* tmp = head;
        head = head->get_next();
        delete tmp;
        
    }
}

int ilist::isEmpty() const {
    if (size==0) return 1;
    return 0;
}

int ilist::getSize() const {
    return size;
}

inode* ilist::get_head() const {
    return head;
}

inode* ilist::get_tail() const {
    return tail;
}


void ilist::push(intersection i){
    inode* tmp = new inode(i);
    if(head == nullptr){
        head = tmp;
        tail = tmp;
    }
    else{
        tmp->set_prev(tail);
        tail->set_next(tmp);
        tail = tmp;
    }
    size++;
}


void ilist::remove(int id_i){
    inode* tmp = head;

    while(tmp != nullptr && tmp->get_i().getid_i() != id_i){
        tmp = tmp->get_next();
    }

    //Không tìm thấy giao lộ cần xóa
    if(tmp == nullptr) return;

    //Nối node phía trước và sau
    if(tmp->get_prev() != nullptr){
        tmp->get_prev()->set_next(tmp->get_next());
    }
    else
        head = tmp->get_next();

    if(tmp->get_next() != nullptr){
        tmp->get_next()->set_prev(tmp->get_prev());
    }
    else tail = tmp->get_prev();


    delete tmp;
    size--;

}