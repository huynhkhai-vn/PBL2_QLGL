#ifndef ROAD_NODE_H
#define ROAD_NODE_H

#include "road.h"

class rnode{
    private:
        road edge;
        rnode *next;
        rnode *prev;
    public:
        rnode();
        rnode(road r);
        ~rnode();

        road &get_r();
        rnode* get_next() const;
        rnode* get_prev() const;

        void set_next(rnode* next);
        void set_prev(rnode* prev);
};


#endif