#ifndef A_LIST_H
#define A_LIST_H

#include "anode.h"

class alist {
    private:
        anode* head;
        anode* tail;
        int size;

    public:
        alist();
        ~alist();

        int isEmpty() const;
        int getSize() const;

        anode* get_head() const;
        anode* get_tail() const;

        void push(int id_i);
        void remove(int id_i);
};


#endif