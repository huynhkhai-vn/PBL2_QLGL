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
        linkedlist() : head(nullptr), tail(nullptr), size(0) {}

        linkedlist(const linkedlist &other) : head(nullptr), tail(nullptr), size(0) {
            AdjNode* current = other.get_head();
            while (current != nullptr) {
                push(current->get_i(), current->get_r());
                current = current->get_next();
            }
        }

        ~linkedlist() {
            while (tail != nullptr) {
                AdjNode* tmp = tail;
                tail = tail->get_prev();
                delete tmp;
            }
        }

        int isEmpty() const { return size == 0 ? 1 : 0; }
        int getSize() const { return size; }

        void push(int vertex_id, const road &edge) {
            AdjNode* tmp = new AdjNode(vertex_id, edge, nullptr, nullptr);
            if (head == nullptr) {
                head = tmp;
                tail = tmp;
            } else {
                tmp->set_prev(tail);
                tail->set_next(tmp);
                tail = tmp;
            }
            size++;
        }

        void remove(int id_r) {
            AdjNode* tmp = head;
            while (tmp != nullptr && tmp->get_r().getid_r() != id_r) {
                tmp = tmp->get_next();
            }

            if (tmp == nullptr) return;

            if (tmp->get_prev() != nullptr) {
                tmp->get_prev()->set_next(tmp->get_next());
            } else {
                head = tmp->get_next();
            }

            if (tmp->get_next() != nullptr) {
                tmp->get_next()->set_prev(tmp->get_prev());
            } else {
                tail = tmp->get_prev();
            }

            delete tmp;
            size--;
        }

        AdjNode* get_head() const { return head; }
        AdjNode* get_tail() const { return tail; }

        linkedlist &operator=(const linkedlist &other) {
            if (this == &other) return *this;

            while (tail != nullptr) {
                AdjNode* tmp = tail;
                tail = tail->get_prev();
                delete tmp;
            }
            head = nullptr;
            tail = nullptr;
            size = 0;

            AdjNode* current = other.get_head();
            while (current != nullptr) {
                push(current->get_i(), current->get_r());
                current = current->get_next();
            }

            return *this;
        }
};

#endif