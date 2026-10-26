#include "linkedlist.h"
#include <iostream>

using namespace std;

linkedlist::linkedlist()
    : head(nullptr), tail(nullptr), size(0) {}



linkedlist::linkedlist(const linkedlist &other)
    : head(nullptr), tail(nullptr), size(0) {

    AdjNode* current = other.get_head();

    while(current != nullptr){
        push(current->get_i(), current->get_r());
        current = current->get_next();
    }
}
    
linkedlist::~linkedlist(){
    while(tail != nullptr){
        AdjNode* tmp = tail;
        tail = tail->get_prev();
        delete tmp;
    }
}

int linkedlist::isEmpty() const {
    if(size == 0) return 1;
    return 0;
}

int linkedlist::getSize() const {
    return size;
}

void linkedlist::push(int vertex_id, const road &edge){
    
    AdjNode* tmp = new AdjNode(vertex_id, edge, nullptr, nullptr);
    
    if(head == nullptr){
        head = tmp;
        tail = tmp;
    }
    else{
        // tmp->get_prev() = this->get_tail();
        // this->get_tail() = tmp;

        tmp->set_prev(tail);
        tail->set_next(tmp);
        tail = tmp;
    }

    size++;
}

void linkedlist::remove(int id_r){
    AdjNode* tmp = head;

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





AdjNode* linkedlist::get_head() const {
    return head;
}

AdjNode* linkedlist::get_tail() const {
    return tail;
}


linkedlist& linkedlist::operator=(const linkedlist& other) {

    if(this == &other)
        return *this;

    while(tail != nullptr){
        AdjNode* tmp = tail;
        tail = tail->get_prev();
        delete tmp;
    }
    head = nullptr;
    tail = nullptr;
    size = 0;

    AdjNode* current = other.get_head();

    while(current != nullptr){
        this->push(current->get_i(), current->get_r());
        current = current->get_next();
    }

    return *this;
}