// 00:16 AM ngày 21/9/2026


#ifndef MAP_H
#define MAP_H

#include "linkedlist.h"
#include <fstream>
#include <string>





using namespace std;

class dijkstra;



class map {
    friend class dijkstra;
    private:
        intersection* intersections;
        int num_i;
        int size_i;
    
        road* roads;
        int num_r;
        int size_r;

        linkedlist* graph;


        


    public:
        map(int size_i = 100, int size_r = 100);
        ~map();
        map(const map&) = delete;
        map& operator=(const map&) = delete;

        bool add_i(intersection i);
        void remove_i(int id_i);

        bool add_r(road r);
        void remove_r(int id_r);

        int get_num_i() const;
        int get_size_i() const;

        int get_num_r() const;
        int get_size_r() const;

        // Truy cap phan tu theo chi so de hien thi ban do tren giao dien.
        const intersection& getIntersectionAt(int index) const;
        const road& getRoadAt(int index) const;
        double getRoadTime(int start_id, int end_id) const;
        double getRoadDistance(int start_id, int end_id) const;
        double getRoadTimeById(int id_r, int start_id, int end_id) const;
        double getRoadDistanceById(int id_r, int start_id, int end_id) const;
        int isEmpty_i() const;
        int isFull_i() const;

        int isEmpty_r() const;
        int isFull_r() const;


        int find_i(int id_i) const;
        int find_r(int id_r) const;

        bool load_i(const string &filename);
        bool load_r(const string &filename);


        void show_i() const;
        void show_r() const;
        void show_map() const;
};





#endif