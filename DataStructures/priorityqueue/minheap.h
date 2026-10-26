#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include "hnode.h"
#include <iostream>
using namespace std;

class minheap{
    private:
        hnode* heap;
        int size;
        int n;

        void heapify_up(int i);
        void heapify_down(int i);
        void swap(hnode &a, hnode &b);


    public:
        minheap(int size = 100);
        ~minheap();
        minheap(const minheap&) = delete;
        minheap& operator=(const minheap&) = delete;

        void push(hnode node);
        hnode pop();
        hnode top() const;
        int isEmpty() const;
};

#endif