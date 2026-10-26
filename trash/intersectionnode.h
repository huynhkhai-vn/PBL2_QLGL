#ifndef INTERSECTION_NODE_H
#define INTERSECTION_NODE_H


#include "intersection.h"




class inode {
    private:
        intersection vertex;
        inode* next;
        inode* prev; 
    public:
        inode();
        inode(intersection i);
        ~inode();

        intersection get_i() const;
        inode* get_next() const;
        inode* get_prev() const;

        void set_next(inode* next);
        void set_prev(inode* prev);
};

#endif