#ifndef HEAP_NODE_H
#define HEAP_NODE_H

class hnode {
    private:
        int id_i;
        double cost;
    public:
        hnode() : id_i(0), cost(0) {}
        hnode(int id_i, double cost) : id_i(id_i), cost(cost) {}
        hnode(const hnode& other) = default;
        ~hnode() = default;

        int getid_i() const { return id_i; }
        double get_cost() const { return cost; }
};

#endif