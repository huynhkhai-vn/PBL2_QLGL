#ifndef ADJNODE_H
#define ADJNODE_H
#include "../road/road.h"
#include "../intersection/intersection.h"

class AdjNode {
    private:
        int vertex_id;
        road edge;
        AdjNode* next;
        AdjNode* prev;
    public:
        AdjNode();
        AdjNode(int vertex_id, road edge, AdjNode* next, AdjNode* prev);
        ~AdjNode();

        //getter
        int get_i() const;     
        const road &get_r() const;
        AdjNode* get_next() const;
        AdjNode* get_prev() const;

        //setter
        void set_next(AdjNode* next);
        void set_prev(AdjNode* prev);

};




#endif