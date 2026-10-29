#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "Map.h"
#include "Vehicle.h"
#include "Package.h"
#include <vector>
#include <string>

class Algorithm {
public:
    // 任务三：送货方案规划
    static int deliveryPlan(const Map& map,
                            const std::vector<Vehicle>& vehicles,
                            const std::vector<Package>& packages,
                            std::vector<std::vector<int>>& routes,
                            std::vector<int>& latePackages);

    // 任务四：退货路线规划
    static int returnPlan(const Map& map,
                          const Vehicle& vehicle,
                          const std::vector<std::string>& returnLocations,
                          std::vector<int>& route);

    // 任务五：充电最少行驶距离
    static void prepareMinMaxEdge(Map& map);

    // 扩展任务一：两辆小车送货
    static int dualVehicleDelivery(const Map& map,
                                   const std::vector<Vehicle>& vehicles,
                                   const std::vector<Package>& packages,
                                   std::vector<std::vector<std::vector<int>>>& routes,
                                   std::vector<int>& latePackages);

    static long long scheduleDeliveries(int n, int K, long long X, const std::vector<long long>& arrivalTimes);

private:
    // 辅助函数声明
    static void processDeliveryBatch(const Map& map,
                                     const Vehicle& vehicle,
                                     std::vector<Package>& batch,
                                     std::vector<std::vector<int>>& routes,
                                     std::vector<int>& latePackages,
                                     int& totalCost);

    static int getBatchWeight(const std::vector<Package>& batch);

    static void calculateDeliveryTimes(const Map& map,
                                       const Vehicle& vehicle,
                                       const std::vector<Package>& batch,
                                       const std::vector<int>& route,
                                       std::vector<int>& latePackages);
    static void processDualVehicleBatch(const Map& map,
                                        const Vehicle& vehicle,
                                        std::vector<Package>& batch,
                                        std::vector<std::vector<int>>& vehicleRoutes,
                                        std::vector<int>& latePackages,
                                        int& totalCost);
};

#endif // ALGORITHM_H
