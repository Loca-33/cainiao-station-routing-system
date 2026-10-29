#ifndef VEHICLE_H
#define VEHICLE_H

#include <vector>
#include "Package.h"

class Vehicle {
public:
    int id;             // 小车ID
    int speed;          // 速度(m/min)
    int weight;         // 自重(kg)
    int maxLoad;        // 最大载重(kg)
    int currentLoad;    // 当前载重(kg)
    int currentLocation;// 当前位置
    std::vector<Package*> packages; // 当前装载的包裹

    Vehicle(int id, int v, int w, int lim)
        : id(id), speed(v), weight(w), maxLoad(lim), currentLoad(0), currentLocation(1) {
    } // 默认起点是1(驿站)

    bool canLoad(Package* pkg) const {
        return currentLoad + pkg->weight <= maxLoad;
    }

    void loadPackage(Package* pkg) {
        packages.push_back(pkg);
        currentLoad += pkg->weight;
    }

    void unloadAllPackages() {
        packages.clear();
        currentLoad = 0;
    }
};

#endif // VEHICLE_H
