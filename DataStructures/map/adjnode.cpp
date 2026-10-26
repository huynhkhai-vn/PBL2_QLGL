#include "adjnode.h"
#include "../intersection/intersection.h"
#include "../road/road.h"



#include <iostream>

using namespace std;

AdjNode::AdjNode() : vertex_id(0), edge(), next(nullptr), prev(nullptr){}

AdjNode::AdjNode(int vertex_id, road edge, AdjNode* next, AdjNode* prev){
    this->vertex_id = vertex_id;
    this->edge = edge;
    this->next = next;
    this->prev = prev;
}

// AdjNode::AdjNode(intersection vertex, road* edge,
//                  AdjNode* next, AdjNode* prev)
//     : vertex(vertex), edge(edge), next(next), prev(prev)
// {
// }

AdjNode::~AdjNode(){
}



//GETTER

int AdjNode::get_i() const {
    return vertex_id;
}     // trả về bản copy, muốn tránh copy cần cải thiện
const road &AdjNode::get_r() const {
    return edge;
}
AdjNode* AdjNode::get_next() const {
    return next;
}
AdjNode* AdjNode::get_prev() const {
    return prev;
}



//SETTER

void AdjNode::set_next(AdjNode* next){
    this->next = next;
}

void AdjNode::set_prev(AdjNode* prev){
    this->prev = prev;
}