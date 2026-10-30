#ifndef ADJNODE_H
#define ADJNODE_H

#include "../road.h"

class AdjNode {
    private:
        int vertex_id;
        road edge;
    public:
        AdjNode();
        AdjNode(int vertex_id, const road& edge);
        ~AdjNode();
        //getter
        int get_i() const;
        const road &get_r() const;
};

AdjNode::AdjNode() : vertex_id(0), edge() {}

AdjNode::AdjNode(int vertex_id, const road& edge)
    : vertex_id(vertex_id), edge(edge) {}

AdjNode::~AdjNode() = default;

int AdjNode::get_i() const { return vertex_id; }
const road& AdjNode::get_r() const { return edge; }


#endif