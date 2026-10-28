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

        void heapify_up(int i) {
            while (i > 0) {
                int parent = (i - 1) / 2;
                if (heap[parent].get_cost() <= heap[i].get_cost()) break;
                swap(heap[parent], heap[i]);
                i = parent;
            }
        }

        void heapify_down(int i) {
            while (true) {
                int left = 2 * i + 1;
                int right = 2 * i + 2;
                int smallest = i;
                if (left < n && heap[left].get_cost() < heap[smallest].get_cost()) {
                    smallest = left;
                }
                if (right < n && heap[right].get_cost() < heap[smallest].get_cost()) {
                    smallest = right;
                }
                if (smallest == i) break;

                swap(heap[i], heap[smallest]);
                i = smallest;
            }
        }

        void swap(hnode &a, hnode &b) {
            hnode tmp = a;
            a = b;
            b = tmp;
        }

    public:
        minheap(int size = 100)
            : heap(new hnode[size > 0 ? size : 1]),
              size(size > 0 ? size : 1),
              n(0) {}
        ~minheap() { delete[] heap; }
        minheap(const minheap&) = delete;
        minheap& operator=(const minheap&) = delete;

        void push(hnode node) {
            if (n == size) {
                int new_size = size * 2;
                hnode* new_heap = new hnode[new_size];
                for (int i = 0; i < n; i++) {
                    new_heap[i] = heap[i];
                }
                delete[] heap;
                heap = new_heap;
                size = new_size;
            }
            heap[n] = node;
            n++;
            heapify_up(n - 1);
        }

        hnode pop() {
            if (n == 0) return hnode(-1, -1);

            hnode result = heap[0];
            swap(heap[0], heap[n - 1]);
            n--;
            heapify_down(0);
            return result;
        }

        hnode top() const {
            if (n == 0) return hnode(-1, -1);
            return heap[0];
        }

        int isEmpty() const { return n == 0 ? 1 : 0; }
};

#endif