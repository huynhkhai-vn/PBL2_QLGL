#ifndef ADJNODE_H
#define ADJNODE_H
#include "../road.h"
#include "../intersection.h"

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

inline AdjNode::AdjNode() : vertex_id(0), edge() {}

inline AdjNode::AdjNode(int vertex_id, const road& edge)
    : vertex_id(vertex_id), edge(edge) {}

inline AdjNode::~AdjNode() = default;

inline int AdjNode::get_i() const { return vertex_id; }
inline const road& AdjNode::get_r() const { return edge; }


#endif