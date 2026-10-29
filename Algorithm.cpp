#include "Algorithm.h"
#include <algorithm>
#include <queue>
#include <climits>
#include <unordered_set>

using namespace std;
int Algorithm::deliveryPlan(const Map& map,
                            const vector<Vehicle>& vehicles,
                            const vector<Package>& packages,
                            vector<vector<int>>& routes,
                            vector<int>& latePackages) {
    if (vehicles.empty() || packages.empty()) return 0;

    // 使用第一辆小车（后续可扩展多车逻辑）
    const Vehicle& vehicle = vehicles[0];
    int totalCost = 0;
    latePackages.clear();

    // 按截止时间排序包裹（紧急的优先配送）
    vector<Package> sortedPackages = packages;
    sort(sortedPackages.begin(), sortedPackages.end(),
         [](const Package& a, const Package& b) {
             return a.deadline < b.deadline;
         });

    // 分组送货
    vector<Package> currentBatch;
    int currentWeight = 0;

    for (const auto& pkg : sortedPackages) {
        if (currentWeight + pkg.weight <= vehicle.maxLoad) {
            currentBatch.push_back(pkg);
            currentWeight += pkg.weight;
        }
        else {
            // 送当前批次
            processDeliveryBatch(map, vehicle, currentBatch, routes, latePackages, totalCost);

            // 开始新批次
            currentBatch.clear();
            currentBatch.push_back(pkg);
            currentWeight = pkg.weight;
        }
    }

    // 送最后一批
    if (!currentBatch.empty()) {
        processDeliveryBatch(map, vehicle, currentBatch, routes, latePackages, totalCost);
    }

    return totalCost;
}

// 辅助函数：处理单个配送批次
void Algorithm::processDeliveryBatch(const Map& map,
                                     const Vehicle& vehicle,
                                     vector<Package>& batch,
                                     vector<vector<int>>& routes,
                                     vector<int>& latePackages,
                                     int& totalCost) {
    // 生成配送路线：驿站 -> 目的地1 -> 目的地2 -> ... -> 驿站
    vector<int> route = map.shortestPath(1, batch[0].destination);

    for (size_t i = 1; i < batch.size(); ++i) {
        auto nextRoute = map.shortestPath(batch[i - 1].destination, batch[i].destination);
        route.insert(route.end(), nextRoute.begin() + 1, nextRoute.end());
    }

    // 返回驿站
    auto returnRoute = map.shortestPath(batch.back().destination, 1);
    route.insert(route.end(), returnRoute.begin() + 1, returnRoute.end());
    routes.push_back(route);

    // 计算成本（使用车辆自重+当前批次重量）
    totalCost += map.calculatePathCost(route, vehicle.weight, batch);

    // 计算每个包裹的准确送达时间
    calculateDeliveryTimes(map, vehicle, batch, route, latePackages);
}

// 辅助函数：计算批次总重量
int Algorithm::getBatchWeight(const vector<Package>& batch) {
    int weight = 0;
    for (const auto& pkg : batch) {
        weight += pkg.weight;
    }
    return weight;
}

// 辅助函数：计算每个包裹的送达时间并检查逾期
void Algorithm::calculateDeliveryTimes(const Map& map,
                                       const Vehicle& vehicle,
                                       const vector<Package>& batch,
                                       const vector<int>& route,
                                       vector<int>& latePackages) {
    vector<int> deliveryTimes(batch.size(), 0);
    int accumulatedTime = 0;
    size_t currentPkgIndex = 0;

    // 遍历路径的每一段
    for (size_t i = 0; i < route.size() - 1; ++i) {
        int u = route[i];
        int v = route[i + 1];

        // 查找当前路段的距离
        int segmentDistance = 0;
        for (const auto& [neighbor, dist] : map.getAdjacent(u)) {
            if (neighbor == v) {
                segmentDistance = dist;
                break;
            }
        }

        // 计算并累加时间
        int segmentTime = segmentDistance / vehicle.speed;
        accumulatedTime += segmentTime;

        // 检查是否到达某个包裹的目的地
        if (currentPkgIndex < batch.size() && v == batch[currentPkgIndex].destination) {
            deliveryTimes[currentPkgIndex] = accumulatedTime;
            currentPkgIndex++;
        }
    }

    // 检查逾期包裹
    for (size_t i = 0; i < batch.size(); ++i) {
        if (deliveryTimes[i] > batch[i].deadline) {
            latePackages.push_back(batch[i].id);
        }
    }
}

int Algorithm::returnPlan(const Map& map,
                          const Vehicle& vehicle,
                          const vector<string>& returnLocations,
                          vector<int>& route) {
    // 转换为地点ID
    vector<int> locations;
    for (const auto& name : returnLocations) {
        int id = map.getLocationId(name);
        if (id != -1) {
            locations.push_back(id);
        }
    }

    if (locations.empty()) {
        return 0;
    }

    // 动态规划解决TSP问题
    const int n = locations.size();
    const int depot = 1; // 驿站ID

    // 预计算所有点之间的最短路径距离
    vector<vector<int>> dist(n + 1, vector<int>(n + 1));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i == j) {
                dist[i][j] = 0;
            }
            else {
                auto path = map.shortestPath(locations[i], locations[j]);
                dist[i][j] = 0;
                for (size_t k = 0; k < path.size() - 1; ++k) {
                    int u = path[k];
                    int v = path[k + 1];
                    for (const auto& [neighbor, w] : map.getAdjacent(u)) {
                        if (neighbor == v) {
                            dist[i][j] += w;
                            break;
                        }
                    }
                }
            }
        }
        // 从驿站到各点的距离
        auto path_to = map.shortestPath(depot, locations[i]);
        dist[n][i] = 0;
        for (size_t k = 0; k < path_to.size() - 1; ++k) {
            int u = path_to[k];
            int v = path_to[k + 1];
            for (const auto& [neighbor, w] : map.getAdjacent(u)) {
                if (neighbor == v) {
                    dist[n][i] += w;
                    break;
                }
            }
        }
        // 从各点到驿站的距离
        auto path_from = map.shortestPath(locations[i], depot);
        dist[i][n] = 0;
        for (size_t k = 0; k < path_from.size() - 1; ++k) {
            int u = path_from[k];
            int v = path_from[k + 1];
            for (const auto& [neighbor, w] : map.getAdjacent(u)) {
                if (neighbor == v) {
                    dist[i][n] += w;
                    break;
                }
            }
        }
    }
    dist[n][n] = 0;

    // DP状态: dp[mask][last]表示访问过mask中的点，最后在last点的最小距离
    const int mask_size = 1 << n;
    vector<vector<int>> dp(mask_size, vector<int>(n, INT_MAX));
    vector<vector<int>> parent(mask_size, vector<int>(n, -1));

    // 初始化：从驿站到每个点的距离
    for (int i = 0; i < n; ++i) {
        dp[1 << i][i] = dist[n][i];
    }

    // 动态规划填表
    for (int mask = 1; mask < mask_size; ++mask) {
        for (int last = 0; last < n; ++last) {
            if (!(mask & (1 << last))) continue;
            if (dp[mask][last] == INT_MAX) continue;

            for (int next = 0; next < n; ++next) {
                if (mask & (1 << next)) continue;

                int new_mask = mask | (1 << next);
                int new_dist = dp[mask][last] + dist[last][next];
                if (new_dist < dp[new_mask][next]) {
                    dp[new_mask][next] = new_dist;
                    parent[new_mask][next] = last;
                }
            }
        }
    }

    // 找到最优解：访问所有点后返回驿站
    int final_mask = (1 << n) - 1;
    int min_dist = INT_MAX;
    int last_point = -1;
    for (int i = 0; i < n; ++i) {
        int total_dist = dp[final_mask][i] + dist[i][n];
        if (total_dist < min_dist) {
            min_dist = total_dist;
            last_point = i;
        }
    }

    // 回溯构建路径
    vector<int> order;
    int current_mask = final_mask;
    int current_point = last_point;
    while (current_point != -1) {
        order.push_back(locations[current_point]);
        int prev_point = parent[current_mask][current_point];
        if (prev_point == -1) break;
        current_mask ^= (1 << current_point);
        current_point = prev_point;
    }
    reverse(order.begin(), order.end());

    // 构建完整路径
    route.clear();
    route.push_back(depot);

    // 从驿站到第一个点
    auto start_path = map.shortestPath(depot, order[0]);
    route.insert(route.end(), start_path.begin() + 1, start_path.end());

    // 中间路径
    for (size_t i = 1; i < order.size(); ++i) {
        auto path = map.shortestPath(order[i - 1], order[i]);
        route.insert(route.end(), path.begin() + 1, path.end());
    }

    // 返回驿站
    auto end_path = map.shortestPath(order.back(), depot);
    route.insert(route.end(), end_path.begin() + 1, end_path.end());

    // 计算总距离
    int totalDistance = 0;
    for (size_t i = 0; i < route.size() - 1; ++i) {
        int u = route[i];
        int v = route[i + 1];
        for (const auto& [neighbor, w] : map.getAdjacent(u)) {
            if (neighbor == v) {
                totalDistance += w;
                break;
            }
        }
    }

    return totalDistance;
}

void Algorithm::prepareMinMaxEdge(Map& map) {
    map.prepareKruskalTree();
}

int Algorithm::dualVehicleDelivery(const Map& map,
                                   const vector<Vehicle>& vehicles,
                                   const vector<Package>& packages,
                                   vector<vector<vector<int>>>& routes,
                                   vector<int>& latePackages) {
    // 检查输入有效性
    if (vehicles.size() < 2 || packages.empty()) return 0;

    int totalCost = 0;
    latePackages.clear();
    routes.resize(2); // 两辆车的路线容器

    // 按截止时间排序包裹（紧急的优先配送）
    vector<Package> sortedPackages = packages;
    sort(sortedPackages.begin(), sortedPackages.end(),
         [](const Package& a, const Package& b) {
             return a.deadline < b.deadline;
         });

    // 分配包裹给两辆车（简单轮流分配）
    vector<vector<Package>> assignments(2);
    for (const auto& pkg : sortedPackages) {
        static int turn = 0;
        assignments[turn].push_back(pkg);
        turn = (turn + 1) % 2;
    }

    // 每辆车独立送货
    for (int vehicleIdx = 0; vehicleIdx < 2; ++vehicleIdx) {
        const Vehicle& vehicle = vehicles[vehicleIdx];
        vector<Package> currentBatch;
        int currentWeight = 0;

        for (const auto& pkg : assignments[vehicleIdx]) {
            if (currentWeight + pkg.weight <= vehicle.maxLoad) {
                currentBatch.push_back(pkg);
                currentWeight += pkg.weight;
            }
            else {
                // 处理当前批次
                processDualVehicleBatch(map, vehicle, currentBatch,
                                        routes[vehicleIdx], latePackages, totalCost);

                // 开始新批次
                currentBatch.clear();
                currentBatch.push_back(pkg);
                currentWeight = pkg.weight;
            }
        }

        // 处理最后一批
        if (!currentBatch.empty()) {
            processDualVehicleBatch(map, vehicle, currentBatch,
                                    routes[vehicleIdx], latePackages, totalCost);
        }
    }

    return totalCost;
}

// 新增辅助函数：处理双车配送的单个批次
void Algorithm::processDualVehicleBatch(const Map& map,
                                        const Vehicle& vehicle,
                                        vector<Package>& batch,
                                        vector<vector<int>>& vehicleRoutes,
                                        vector<int>& latePackages,
                                        int& totalCost) {
    if (batch.empty()) return;

    // 生成配送路线
    vector<int> route = map.shortestPath(1, batch[0].destination);
    for (size_t i = 1; i < batch.size(); ++i) {
        auto nextRoute = map.shortestPath(batch[i - 1].destination, batch[i].destination);
        route.insert(route.end(), nextRoute.begin() + 1, nextRoute.end());
    }
    auto returnRoute = map.shortestPath(batch.back().destination, 1);
    route.insert(route.end(), returnRoute.begin() + 1, returnRoute.end());
    vehicleRoutes.push_back(route);

    // 计算成本（保持整数，假设 calculatePathCost 返回 int）
    totalCost += map.calculatePathCost(route, vehicle.weight, batch);

    // 记录每个目的地的到达时间（使用 double 精度）
    unordered_map<int, double> destinationToTime;
    double accumulatedTime = 0.0; // 初始时间为 0.0
    for (size_t i = 0; i < route.size() - 1; ++i) {
        int u = route[i];
        int v = route[i + 1];
        int segmentDistance = 0;
        for (const auto& [neighbor, dist] : map.getAdjacent(u)) {
            if (neighbor == v) {
                segmentDistance = dist;
                break;
            }
        }
        // 直接使用浮点除法计算时间
        double segmentTime = static_cast<double>(segmentDistance) / vehicle.speed;
        accumulatedTime += segmentTime;
        destinationToTime[v] = accumulatedTime;
    }

    // 检查逾期包裹（比较 double 时间）
    for (const auto& pkg : batch) {
        if (destinationToTime[pkg.destination] > static_cast<double>(pkg.deadline)) {
            latePackages.push_back(pkg.id);
        }
    }
}

long long Algorithm::scheduleDeliveries(int n, int K, long long X, const std::vector<long long>& arrivalTimes) {
    if (n == 0) return 0;

    vector<long long> sortedTimes = arrivalTimes;
    sort(sortedTimes.begin(), sortedTimes.end());

    // dp[i].first = 最小总不满意度
    // dp[i].second = 最后发货时间
    vector<pair<long long, long long>> dp(n + 1, { LLONG_MAX, 0 });  // 改为long long
    dp[0] = { 0, -X }; // 初始状态

    for (int i = 1; i <= n; ++i) {
        // 尝试所有可能的批次大小(1..K)
        for (int j = 1; j <= K && i - j >= 0; ++j) {
            // 这批包裹的最早可能发货时间
            long long dispatchTime = max(sortedTimes[i - 1], dp[i - j].second + X);

            // 计算这批包裹的不满意度
            long long dissatisfaction = 0;
            for (int k = i - j; k < i; ++k) {
                dissatisfaction += dispatchTime - sortedTimes[k];
            }

            // 如果发现更优解
            if (dp[i - j].first + dissatisfaction < dp[i].first) {
                dp[i].first = dp[i - j].first + dissatisfaction;
                dp[i].second = dispatchTime;

                // 特别优化：如果发现0不满意度，可以提前终止
                if (dp[i].first == 0 && i == n) {
                    return 0;
                }
            }
        }
    }

    return dp[n].first;
}
