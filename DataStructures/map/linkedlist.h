#ifndef LINKED_LIST_H
#define LINKED_LIST_H
#include "adjnode.h"
#include <iostream>
using namespace std;

class linkedlist {
    private:
        AdjNode* head;
        AdjNode* tail;
        int size;
    public:
        linkedlist();
        linkedlist(const linkedlist &other);
        ~linkedlist();

        int isEmpty() const;
        int getSize() const;

        void push(int vertex_id, const road &edge);
        void remove(int id_r);

        AdjNode* get_head() const;
        AdjNode* get_tail() const;

        linkedlist &operator=(const linkedlist &other);
};

#endif