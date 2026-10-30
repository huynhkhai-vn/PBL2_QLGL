#ifndef HEAP_NODE_H
#define HEAP_NODE_H

class hnode {
    private:
        int id_i;
        double cost;
    public:
        hnode();
        hnode(int id_i, double cost);
        hnode(const hnode& other);
        hnode& operator=(const hnode& other);
        ~hnode();

        int getid_i() const;
        double get_cost() const;
};

hnode::hnode() : id_i(0), cost(0) {}
hnode::hnode(int id_i, double cost) : id_i(id_i), cost(cost) {}
hnode::hnode(const hnode& other) = default;
hnode& hnode::operator=(const hnode& other) = default;
hnode::~hnode() = default;

int hnode::getid_i() const { return id_i; }
double hnode::get_cost() const { return cost; }

#endif