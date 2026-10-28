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

inline hnode::hnode() : id_i(0), cost(0) {}
inline hnode::hnode(int id_i, double cost) : id_i(id_i), cost(cost) {}
inline hnode::hnode(const hnode& other) = default;
inline hnode& hnode::operator=(const hnode& other) = default;
inline hnode::~hnode() = default;

inline int hnode::getid_i() const { return id_i; }
inline double hnode::get_cost() const { return cost; }

#endif