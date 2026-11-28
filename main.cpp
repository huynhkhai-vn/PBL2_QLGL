#include <iostream>
#include <exception>
#include <vector>
#include "include/DAL/map_data_access.h"
#include "dijkstra.h"
#include "taxi.h"

using namespace std;

int runConsole() {
    cout << "===== BAT DAU KIEM TRA PBL2 =====" << endl;

    // 1. Khoi tao ban do va doc du lieu
    map m;
    MapDataAccess dataAccess;
    string error;
    if (!dataAccess.load(m, error)) {
        cerr << error << endl;
        return 1;
    }

    cout << "So giao lo: " << m.get_num_i() << endl;
    cout << "So con duong: " << m.get_num_r() << endl;

    // 2. Khoi tao thuat toan Dijkstra va taxi
    dijkstra dj;
    taxi t(101, 1);

    vector<int> path(m.get_num_i());
    vector<int> road_ids(m.get_num_i() > 0 ? m.get_num_i() - 1 : 0);
    int count = 0;

    // 3. Kiem tra tim duong ngan nhat tu 1 den 10
    cout << endl;
    cout << "===== TEST 1: DUONG NGAN NHAT =====" << endl;

    dj.shortestPath(m, 1, 10, path.data(), count, road_ids.data());

    if (count > 0) {
        t.setPath(path.data(), count, road_ids.data());

        while (t.getstatus() == 1) {
            t.move(m);

            cout << "Taxi dang o giao lo: "
                 << t.getcurrent_id() << endl;
        }
    }

    cout << "Tong thoi gian: "
         << t.gettotal_time() << endl;

    cout << "Tong quang duong: "
         << t.gettotal_distance() << endl;

    // 4. Kiem tra tim duong nhanh nhat tu vi tri hien tai den 5
    cout << endl;
    cout << "===== TEST 2: DUONG NHANH NHAT =====" << endl;

    dj.fastestPath(m, t.getcurrent_id(), 5, path.data(), count, road_ids.data());

    if (count > 0) {
        t.setPath(path.data(), count, road_ids.data());

        while (t.getstatus() == 1) {
            t.move(m);

            cout << "Taxi dang o giao lo: "
                 << t.getcurrent_id() << endl;
        }
    }

    // 5. In ket qua cuoi cung
    cout << endl;
    cout << "===== KET QUA CUOI CUNG =====" << endl;

    cout << "Vi tri taxi: "
         << t.getcurrent_id() << endl;

    cout << "Tong thoi gian: "
         << t.gettotal_time() << endl;

    cout << "Tong quang duong: "
         << t.gettotal_distance() << endl;

    cout << endl;
    cout << "===== KET THUC KIEM TRA =====" << endl;

    return 0;
}

int main() {
    try {
        return runConsole();
    } catch (const exception& error) {
        cerr << "Loi ngoai le: " << error.what() << endl;
    } catch (...) {
        cerr << "Loi khong xac dinh." << endl;
    }
    return 1;
}
