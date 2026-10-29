#include "Map.h"
#include <algorithm>
#include <queue>
#include <limits>
#include "Package.h"
#include<vector>
#include<cmath>
#include <climits>
using std::vector;
using std::max;

// 并查集数据结构
class UnionFind {
private:
    std::vector<int> parent;
    std::vector<int> rank;

public:
    UnionFind(int n) {
        parent.resize(n + 1);
        rank.resize(n + 1, 0);
        for (int i = 0; i <= n; ++i) {
            parent[i] = i;
        }
    }

    int find(int u) {
        if (parent[u] != u) {
            parent[u] = find(parent[u]);
        }
        return parent[u];
    }

    bool unite(int u, int v) {
        u = find(u);
        v = find(v);
        if (u == v) return false;

        if (rank[u] < rank[v]) {
            parent[u] = v;
        } else {
            parent[v] = u;
            if (rank[u] == rank[v]) {
                rank[u]++;
            }
        }
        return true;
    }
};

void Map::initialize(int n) {
    numLocations = n;
    locationNames.resize(n + 1);
    adjList.resize(n + 1);
}

void Map::addLocation(int id, const std::string& name) {
    if (id > 0 && id <= numLocations) {
        locationNames[id] = name;
        nameToId[name] = id;
    }
}

void Map::addRoad(int u, int v, int w) {
    if (u > 0 && u <= numLocations && v > 0 && v <= numLocations) {
        adjList[u].emplace_back(v, w);
        adjList[v].emplace_back(u, w);
    }
}

int Map::getLocationId(const std::string& name) const {
    auto it = nameToId.find(name);
    return it != nameToId.end() ? it->second : -1;
}

std::string Map::getLocationName(int id) const {
    if (id > 0 && id <= numLocations) {
        return locationNames[id];
    }
    return "";
}

const std::vector<std::pair<int, int>>& Map::getAdjacent(int u) const {
    static const std::vector<std::pair<int, int>> empty;
    if (u > 0 && u <= numLocations) {
        return adjList[u];
    }
    return empty;
}

std::vector<int> Map::shortestPath(int start, int end) const {
    std::vector<int> dist(numLocations + 1, INT_MAX);
    std::vector<int> prev(numLocations + 1, -1);
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, std::greater<>> pq;

    dist[start] = 0;
    pq.emplace(0, start);

    while (!pq.empty()) {
        auto [currentDist, u] = pq.top();
        pq.pop();

        if (u == end) break;
        if (currentDist > dist[u]) continue;

        for (const auto& [v, weight] : adjList[u]) {
            if (dist[v] > dist[u] + weight) {
                dist[v] = dist[u] + weight;
                prev[v] = u;
                pq.emplace(dist[v], v);
            }
        }
    }

    std::vector<int> path;
    if (dist[end] != INT_MAX) {
        for (int at = end; at != -1; at = prev[at]) {
            path.push_back(at);
        }
        std::reverse(path.begin(), path.end());
    }
    return path;
}

int Map::calculatePathCost(const std::vector<int>& path,
                           int vehicleWeight,
                           const std::vector<Package>& batch) const {
    if (path.size() < 2) return 0;

    int cost = 0;
    int currentWeight = vehicleWeight;
    for (const auto& pkg : batch) {
        currentWeight += pkg.weight;
    }

    size_t nextDeliveryIndex = 0;

    for (size_t i = 0; i < path.size() - 1; ++i) {
        int u = path[i];
        int v = path[i + 1];

        int edgeWeight = 0;
        for (const auto& [neighbor, w] : adjList[u]) {
            if (neighbor == v) {
                edgeWeight = w;
                break;
            }
        }

        cost += edgeWeight * currentWeight;

        if (nextDeliveryIndex < batch.size() && v == batch[nextDeliveryIndex].destination) {
            currentWeight -= batch[nextDeliveryIndex].weight;
            nextDeliveryIndex++;
        }
    }

    return cost;
}

void Map::prepareKruskalTree() {
    // 构建Kruskal重构树
    std::vector<std::pair<int, std::pair<int, int>>> edges;
    for (int u = 1; u <= numLocations; ++u) {
        for (const auto& [v, w] : adjList[u]) {
            if (u < v) { // 避免重复添加边
                edges.emplace_back(w, std::make_pair(u, v));
            }
        }
    }

    // 按边权排序
    std::sort(edges.begin(), edges.end());

    // 初始化并查集
    UnionFind uf(numLocations * 2); // 重构树最多有2n-1个节点

    // 初始化Kruskal树
    kruskalTree.resize(numLocations * 2);
    for (auto& node : kruskalTree) {
        node.resize(2, -1); // 每个节点有两个子节点
    }

    // 初始化父节点和maxEdge
    parent.resize(20, std::vector<int>(numLocations * 2, -1));
    maxEdge.resize(20, std::vector<int>(numLocations * 2, 0));
    depth.resize(numLocations * 2, 0);

    int newNode = numLocations;
    for (const auto& [w, edge] : edges) {
        int u = edge.first;
        int v = edge.second;

        int rootU = uf.find(u);
        int rootV = uf.find(v);

        if (rootU != rootV) {
            newNode++;
            kruskalTree[newNode][0] = rootU;
            kruskalTree[newNode][1] = rootV;
            maxEdge[0][newNode] = w;

            parent[0][rootU] = newNode;
            parent[0][rootV] = newNode;

            uf.unite(rootU, newNode);
            uf.unite(rootV, newNode);
        }
    }

    // 构建稀疏表
    for (int k = 1; k < 20; ++k) {
        for (int u = 1; u <= newNode; ++u) {
            if (parent[k-1][u] != -1) {
                parent[k][u] = parent[k-1][parent[k-1][u]];
                maxEdge[k][u] = std::max(maxEdge[k-1][u], maxEdge[k-1][parent[k-1][u]]);
            }
        }
    }

    // 计算深度
    for (int u = newNode; u >= 1; --u) {
        if (parent[0][u] != -1) {
            depth[u] = depth[parent[0][u]] + 1;
        }
    }
}

int Map::getMinMaxEdge(int x, int y) const {
    if (x == y) return 0;

    // 找到x和y的LCA
    int a = x, b = y;
    if (depth[a] < depth[b]) std::swap(a, b);

    int maxW = 0;

    // 提升a到与b相同深度
    for (int k = 19; k >= 0; --k) {
        if (depth[a] - (1 << k) >= depth[b]) {
            maxW = std::max(maxW, maxEdge[k][a]);
            a = parent[k][a];
        }
    }

    if (a == b) return maxW;

    // 同时提升a和b
    for (int k = 19; k >= 0; --k) {
        if (parent[k][a] != -1 && parent[k][a] != parent[k][b]) {
            maxW = std::max(maxW, maxEdge[k][a]);
            maxW = std::max(maxW, maxEdge[k][b]);
            a = parent[k][a];
            b = parent[k][b];
        }
    }

    maxW = std::max(maxW, maxEdge[0][a]);
    maxW = std::max(maxW, maxEdge[0][b]);

    return maxW;
}
