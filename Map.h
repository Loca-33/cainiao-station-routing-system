#ifndef MAP_H
#define MAP_H

#include <vector>
#include <string>
#include <unordered_map>
#include <climits>
#include "Package.h"

class Map {
private:
    int numLocations;
    std::vector<std::string> locationNames;
    std::unordered_map<std::string, int> nameToId;
    std::vector<std::vector<std::pair<int, int>>> adjList;

    // 用于任务五的算法 - 改为稀疏表存储
    std::vector<std::vector<int>> parent;  // 用于Kruskal重构树
    std::vector<std::vector<int>> maxEdge; // 稀疏表
    std::vector<int> depth;                // 节点深度
    std::vector<std::vector<int>> kruskalTree; // Kruskal重构树

public:
    Map() : numLocations(0) {}

    void initialize(int n);
    void addLocation(int id, const std::string& name);
    void addRoad(int u, int v, int w);
    int getLocationId(const std::string& name) const;
    std::string getLocationName(int id) const;
    const std::vector<std::pair<int, int>>& getAdjacent(int u) const;
    int getNumLocations() const { return numLocations; }

    // 任务五相关方法 - 修改为Kruskal重构树方法
    void prepareKruskalTree();
    int getMinMaxEdge(int x, int y) const;

    // 路径规划辅助方法
    std::vector<int> shortestPath(int start, int end) const;
    int calculatePathCost(const std::vector<int>& path,
                          int vehicleWeight,
                          const std::vector<Package>& batch) const;
};

#endif // MAP_H
