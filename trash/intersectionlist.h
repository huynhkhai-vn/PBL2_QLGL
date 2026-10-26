#ifndef INTERSECTION_LIST_H
#define INTERSECTION_LIST_H

#include "intersectionnode.h"

class ilist{
    private:
        inode* head;
        inode* tail;
        int size;
    public:
        ilist();
        ~ilist();

        int isEmpty() const;
        int getSize() const;


        inode* get_head() const;
        inode* get_tail() const;

        void push(intersection i);

        void remove(int id_i);
};


#endif