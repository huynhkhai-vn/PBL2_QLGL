#include "roadlist.h"



rlist::rlist()
    : head(nullptr), tail(nullptr), size(0){}

rlist::~rlist(){
    while(head != nullptr){
        rnode* tmp = head;
        head = head->get_next();
        delete tmp;
    }
}

int rlist::isEmpty() const {
    if (size == 0) return 1;
    return 0;
}


int rlist::getSize() const {
    return size;
}

rnode* rlist::get_head() const {
    return head;
}

rnode* rlist::get_tail() const {
    return tail;
}

void rlist::push(road r){
    rnode* tmp = new rnode(r);
    if(head == nullptr){
        head = tmp;
        tail = tmp;
    }
    else{
        tmp->set_prev(tail);
        tail->set_prev(tmp);
        tail = tmp;
    }
    size++;
}

void rlist::remove(int id_r){
    rnode* tmp = head;

    while(tmp != nullptr && tmp->get_r().getid_r() != id_r){
        tmp = tmp->get_next();
    }

    //Không tìm thấy con đường cần xóa
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