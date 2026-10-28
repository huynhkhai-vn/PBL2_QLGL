#ifndef ADJNODE_H
#define ADJNODE_H
#include "../road.h"
#include "../intersection.h"

class AdjNode {
    private:
        int vertex_id;
        road edge;
        AdjNode* next;
        AdjNode* prev;
    public:
        AdjNode() : vertex_id(0), edge(), next(nullptr), prev(nullptr) {}
        AdjNode(int vertex_id, road edge, AdjNode* next, AdjNode* prev)
            : vertex_id(vertex_id), edge(edge), next(next), prev(prev) {}
        ~AdjNode() = default;

        //getter
        int get_i() const { return vertex_id; }
        const road &get_r() const { return edge; }
        AdjNode* get_next() const { return next; }
        AdjNode* get_prev() const { return prev; }

        //setter
        void set_next(AdjNode* next) { this->next = next; }
        void set_prev(AdjNode* prev) { this->prev = prev; }

};




#endif