#ifndef FILE_IO_H
#define FILE_IO_H

#include "Map.h"
#include "Vehicle.h"
#include "Package.h"
#include <string>
#include <vector>

class FileIO {
public:
    static bool loadMapData(const std::string& filename, Map& map);
    static bool loadTask3Data(const std::string& filename,
                              std::vector<Vehicle>& vehicles,
                              std::vector<Package>& packages);
    static bool loadTask4Data(const std::string& filename,
                              std::vector<Vehicle>& vehicles,
                              std::vector<std::string>& returnLocations);
    static bool loadTask5Data(const std::string& filename,
                              Map& map,
                              std::vector<std::pair<int, int>>& queries);
    static bool loadExtensionTask1Data(const std::string& filename,
                                       std::vector<Vehicle>& vehicles,
                                       std::vector<Package>& packages);
    static bool loadExtensionTask2Data(const std::string& filename,
                                       int& n, int& K, long long& X,
                                       std::vector<long long>& arrivalTimes);

    static void saveTask3Results(const std::string& filename,
                                 int totalCost,
                                 const std::vector<int>& latePackages);
    static void saveTask4Results(const std::string& filename,
                                 int totalDistance,
                                 const std::vector<int>& route,
                                 const Map& map);
    static void saveTask5Results(const std::string& filename,
                                 const std::vector<int>& results);
    static void saveExtensionTask1Results(const std::string& filename,
                                          int totalCost,
                                          const std::vector<int>& latePackages);
    static void saveExtensionTask2Results(const std::string& filename,
                                          long long totalDissatisfaction);
};

#endif // FILE_IO_H
