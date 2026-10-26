#ifndef ROAD_LIST_H
#define ROAD_LIST_H


#include "roadnode.h"

class rlist{
    private:
        rnode* head;
        rnode* tail;
        int size;

    public:
        rlist();
        ~rlist();

        int isEmpty() const;
        int getSize() const;

        rnode* get_head() const;
        rnode* get_tail() const;


        void push(road r);

        void remove(int id_r);
};

#endif