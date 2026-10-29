#include "FileIO.h"
#include "Utils.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <filesystem>

using namespace std;

bool FileIO::loadMapData(const string& filename, Map& map) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening map file: " << filename << endl;
        cerr << "Current working directory: " << filesystem::current_path() << endl;
        return false;
    }

    string line;
    // 读取n和m
    if (!getline(inFile, line)) {
        cerr << "Failed to read n and m values" << endl;
        return false;
    }
    auto nm = split(line, ' ');
    if (nm.size() != 2) {
        cerr << "Invalid format for n and m values: " << line << endl;
        return false;
    }

    try {
        int n = stoi(nm[0]);
        int m = stoi(nm[1]);
        map.initialize(n);

        // 读取n个地点名称
        for (int i = 1; i <= n; ++i) {
            if (!getline(inFile, line)) {
                cerr << "Failed to read location name for id " << i << endl;
                return false;
            }
            map.addLocation(i, trim(line));
        }

        // 读取m条道路
        for (int i = 0; i < m; ++i) {
            if (!getline(inFile, line)) {
                cerr << "Failed to read road " << (i+1) << " of " << m << endl;
                return false;
            }
            auto uvw = split(line, ' ');
            if (uvw.size() != 3) {
                cerr << "Invalid road format: " << line << endl;
                return false;
            }

            int u = stoi(uvw[0]);
            int v = stoi(uvw[1]);
            int w = stoi(uvw[2]);
            map.addRoad(u, v, w);
        }
    } catch (const exception& e) {
        cerr << "Error parsing map data: " << e.what() << endl;
        return false;
    }

    inFile.close();
    return true;
}

bool FileIO::loadTask3Data(const string& filename,
                           vector<Vehicle>& vehicles,
                           vector<Package>& packages) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening Task3 data file: " << filename << endl;
        return false;
    }

    string line;
    // 读取小车数量
    if (!getline(inFile, line)) return false;
    int numCar = stoi(line);

    // 读取小车信息
    for (int i = 0; i < numCar; ++i) {
        if (!getline(inFile, line)) return false;
        auto carInfo = split(line, ' ');
        if (carInfo.size() != 3) return false;

        int v = stoi(carInfo[0]);
        int w_car = stoi(carInfo[1]);
        int w_lim = stoi(carInfo[2]);
        vehicles.emplace_back(i + 1, v, w_car, w_lim);
    }

    // 读取包裹数量
    if (!getline(inFile, line)) return false;
    int k = stoi(line);

    // 读取包裹信息
    for (int i = 0; i < k; ++i) {
        if (!getline(inFile, line)) return false;
        auto pkgInfo = split(line, ' ');
        if (pkgInfo.size() != 5) return false;

        int id = stoi(pkgInfo[0]);
        int weight = stoi(pkgInfo[1]);
        int dest = stoi(pkgInfo[2]);
        int arrival = stoi(pkgInfo[3]);
        int deadline = stoi(pkgInfo[4]);
        packages.emplace_back(id, weight, dest, arrival, deadline);
    }

    inFile.close();
    return true;
}

bool FileIO::loadTask4Data(const string& filename,
                           vector<Vehicle>& vehicles,
                           vector<string>& returnLocations) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening Task4 data file: " << filename << endl;
        return false;
    }

    string line;
    // 读取小车数量
    if (!getline(inFile, line)) return false;
    int numCar = stoi(line);

    // 读取小车信息
    for (int i = 0; i < numCar; ++i) {
        if (!getline(inFile, line)) return false;
        auto carInfo = split(line, ' ');
        if (carInfo.size() != 3) return false;

        int v = stoi(carInfo[0]);
        int w_car = stoi(carInfo[1]);
        int w_lim = stoi(carInfo[2]);
        vehicles.emplace_back(i + 1, v, w_car, w_lim);
    }

    // 读取退货地点数量
    if (!getline(inFile, line)) return false;
    int k = stoi(line);

    // 读取退货地点名称
    if (!getline(inFile, line)) return false;
    auto locations = split(line, ' ');
    returnLocations = vector<string>(locations.begin(), locations.end());

    inFile.close();
    return true;
}

bool FileIO::loadTask5Data(const string& filename,
                           Map& map,
                           vector<pair<int, int>>& queries) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening Task5 data file: " << filename << endl;
        return false;
    }

    string line;
    // 读取n和m
    if (!getline(inFile, line)) return false;
    auto nm = split(line, ' ');
    if (nm.size() != 2) return false;

    int n = stoi(nm[0]);
    int m = stoi(nm[1]);
    map.initialize(n);

    // 读取m条道路
    for (int i = 0; i < m; ++i) {
        if (!getline(inFile, line)) return false;
        auto uvw = split(line, ' ');
        if (uvw.size() != 3) return false;

        int u = stoi(uvw[0]);
        int v = stoi(uvw[1]);
        int w = stoi(uvw[2]);
        map.addRoad(u, v, w);
    }

    // 读取查询数量Q
    if (!getline(inFile, line)) return false;
    int Q = stoi(line);

    // 读取查询
    for (int i = 0; i < Q; ++i) {
        if (!getline(inFile, line)) return false;
        auto xy = split(line, ' ');
        if (xy.size() != 2) return false;

        int x = stoi(xy[0]);
        int y = stoi(xy[1]);
        queries.emplace_back(x, y);
    }

    inFile.close();
    return true;
}

bool FileIO::loadExtensionTask1Data(const string& filename,
                                    vector<Vehicle>& vehicles,
                                    vector<Package>& packages) {
    // 类似于Task3，但可能有更多小车
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening Extension Task1 data file: " << filename << endl;
        return false;
    }

    string line;
    // 读取小车数量
    if (!getline(inFile, line)) return false;
    int numCar = stoi(line);

    // 读取小车信息
    for (int i = 0; i < numCar; ++i) {
        if (!getline(inFile, line)) return false;
        auto carInfo = split(line, ' ');
        if (carInfo.size() != 3) return false;

        int v = stoi(carInfo[0]);
        int w_car = stoi(carInfo[1]);
        int w_lim = stoi(carInfo[2]);
        vehicles.emplace_back(i + 1, v, w_car, w_lim);
    }

    // 读取包裹数量
    if (!getline(inFile, line)) return false;
    int k = stoi(line);

    // 读取包裹信息
    for (int i = 0; i < k; ++i) {
        if (!getline(inFile, line)) return false;
        auto pkgInfo = split(line, ' ');
        if (pkgInfo.size() != 5) return false;

        int id = stoi(pkgInfo[0]);
        int weight = stoi(pkgInfo[1]);
        int dest = stoi(pkgInfo[2]);
        int arrival = stoi(pkgInfo[3]);
        int deadline = stoi(pkgInfo[4]);
        packages.emplace_back(id, weight, dest, arrival, deadline);
    }

    inFile.close();
    return true;
}

bool FileIO::loadExtensionTask2Data(const string& filename,
                                    int& n, int& K, long long& X,
                                    vector<long long>& arrivalTimes) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Error opening Extension Task2 data file: " << filename << endl;
        return false;
    }

    string line;
    // 读取n, K, X
    if (!getline(inFile, line)) return false;
    auto nkx = split(line, ' ');
    if (nkx.size() != 3) return false;

    n = stoi(nkx[0]);
    K = stoi(nkx[1]);
    X = stoll(nkx[2]);

    // 读取到达时间
    if (!getline(inFile, line)) return false;
    auto times = split(line, ' ');
    arrivalTimes.clear();
    for (const auto& t : times) {
        arrivalTimes.push_back(stoll(t));  // 使用 stoll 代替 stoi
    }
    inFile.close();
    return true;
}

void FileIO::saveTask3Results(const string& filename,
                              int totalCost,
                              const vector<int>& latePackages) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    outFile << "Total Cost: " << totalCost << endl;
    outFile << "Late Packages: ";
    for (int id : latePackages) {
        outFile << id << " ";
    }
    outFile << endl;

    outFile.close();
}

void FileIO::saveTask4Results(const string& filename,
                              int totalDistance,
                              const vector<int>& route,
                              const Map& map) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    outFile << "Total Distance: " << totalDistance << endl;
    outFile << "Path: ";
    for (size_t i = 0; i < route.size(); ++i) {
        if (i != 0) outFile << " -> ";
        outFile << map.getLocationName(route[i]);
    }
    outFile << endl;

    outFile.close();
}

void FileIO::saveTask5Results(const string& filename,
                              const vector<int>& results) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    for (int res : results) {
        outFile << res << endl;
    }

    outFile.close();
}

void FileIO::saveExtensionTask1Results(const string& filename,
                                       int totalCost,
                                       const vector<int>& latePackages) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    outFile << "Total Cost: " << totalCost << endl;
    outFile << "Late Packages: ";
    for (int id : latePackages) {
        outFile << id << " ";
    }
    outFile << endl;

    outFile.close();
}

void FileIO::saveExtensionTask2Results(const string& filename,
                                       long long totalDissatisfaction) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error opening output file: " << filename << endl;
        return;
    }

    outFile << "Total Dissatisfaction: " << totalDissatisfaction << endl;

    outFile.close();
}
