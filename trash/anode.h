#ifndef A_NODE_H
#define A_NODE_H

#include "linkedlist.h"

class anode {
    private:
        int id_i;
        linkedlist adjlist;
        
        anode* next;
        anode* prev;
    public:
        anode(int id_i=0);
        ~anode();

        int get_id() const;
        linkedlist& get_list();
        
        anode* get_next() const;
        anode* get_prev() const;

        void set_next(anode* next) ;
        void set_prev(anode* prev) ;

        
};

#endif