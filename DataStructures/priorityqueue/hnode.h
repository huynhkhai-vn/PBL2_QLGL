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
        ~hnode();

        int getid_i() const;
        double get_cost() const;
};

#endif