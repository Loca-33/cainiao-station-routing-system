#ifndef PACKAGE_H
#define PACKAGE_H

#include <string>

struct Package {
    int id;             // 包裹编号
    int weight;         // 包裹重量(kg)
    int destination;    // 目的地节点编号
    int arrivalTime;    // 到达驿站时间(min)
    int deadline;       // 最晚送达时间(min)
    int deliveryTime;   // 实际送达时间(min)
    bool delivered;     // 是否已送达
    bool isReturn;      // 是否是退货包裹

    Package(int id, int w, int dest, int s, int t)
        : id(id), weight(w), destination(dest),
        arrivalTime(s), deadline(t), delivered(false), isReturn(false) {
    }
};

#endif // PACKAGE_H
