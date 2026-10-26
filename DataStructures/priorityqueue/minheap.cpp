#include "minheap.h"

minheap::minheap(int size)
    : heap(new hnode[size > 0 ? size : 1]),
      size(size > 0 ? size : 1),
      n(0) {
}

minheap::~minheap(){
    delete [] heap;
}


int minheap::isEmpty() const {
    if(n==0) return 1;
    return 0;
}


void minheap::swap(hnode &a, hnode &b){
    hnode tmp = a;
    a = b;
    b = tmp;
}





void minheap::heapify_up(int i) {
    while(i>0) {
        int parent = (i-1)/2;
        if(heap[parent].get_cost() <= heap[i].get_cost()) break;
        swap(heap[parent], heap[i]);
        i=parent;
    }
}

void minheap::heapify_down(int i) {
    while(true){
        int l = 2*i + 1;
        int r = 2*i + 2;
        int tmp = i;
        if(l < n && heap[l].get_cost() < heap[tmp].get_cost()) tmp = l;
        if(r < n && heap[r].get_cost() < heap[tmp].get_cost()) tmp = r;

        if(tmp == i ) break;

        swap(heap[i], heap[tmp]);

        i = tmp;
    }
}



void minheap::push(hnode node){
    if(n == size){
        int new_size = size * 2;
        hnode* new_heap = new hnode[new_size];
        for(int i = 0; i < n; i++){
            new_heap[i] = heap[i];
        }
        delete[] heap;
        heap = new_heap;
        size = new_size;
    }
    heap[n] = node;
    n++;
    heapify_up(n-1);
}



hnode minheap::pop() {
    if(n==0) return hnode(-1,-1);

    hnode a = heap[0];
    swap(heap[0], heap[n-1]);
    n--;
    heapify_down(0);
    return a;
}

hnode minheap::top() const {
    if(n==0) return hnode(-1,-1);
    else return heap[0];
}