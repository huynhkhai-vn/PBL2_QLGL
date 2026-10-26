#include "alist.h"


alist::alist()
    : head(nullptr), tail(nullptr), size(0){}

alist::~alist(){
    while(head != nullptr){
        anode* tmp = head;
        head = head->get_next();
        delete tmp;
    }
}

int alist::isEmpty() const {
    if (size == 0) return 1;
    return 0;
}

int alist::getSize() const {
    return size;
}

anode* alist::get_head() const {
    return head;
}

anode* alist::get_tail() const {
    return tail;
}

void alist::push(int id_i){
    anode* tmp = new anode(id_i);
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


void alist::remove(int id_i){
    anode* tmp = head;

    while(tmp != nullptr && tmp->get_id() != id_i){
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