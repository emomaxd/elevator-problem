#include <iostream>
#include <vector>
#include <queue>
#include <set>
#include <algorithm>
#include <cstdlib>
#include <chrono>
#include <fstream>
#include "../dependencies/json.hpp"

using json = nlohmann::json;

const double FLOOR_TRANSIT_TIME = 2.0;
const double DOOR_OPERATION_TIME = 3.0;

struct Elevator {
    int currentFloor;
    double elapsedTime;

    Elevator(int startFloor) : currentFloor(startFloor), elapsedTime(0) {}

    void moveTo(int targetFloor, bool stop) {
        elapsedTime += std::abs(targetFloor - currentFloor) * FLOOR_TRANSIT_TIME;
        if (stop) elapsedTime += DOOR_OPERATION_TIME;
        currentFloor = targetFloor;
    }
};

void fifo(const std::vector<int>& requests, std::vector<Elevator>& elevators) {
    size_t elevatorIndex = 0;
    for (int request : requests) {
        Elevator& elevator = elevators[elevatorIndex];
        elevator.moveTo(request, true);
        elevatorIndex = (elevatorIndex + 1) % elevators.size();
    }
}

void scan(std::vector<int> floors, std::vector<Elevator>& elevators) {
    std::sort(floors.begin(), floors.end());
    for (Elevator& elevator : elevators) {
        for (int floor : floors) {
            elevator.moveTo(floor, true);
        }
    }
}

void look(std::vector<int> floors, std::vector<Elevator>& elevators) {
    std::sort(floors.begin(), floors.end());
    for (Elevator& elevator : elevators) {
        auto pivot = std::lower_bound(floors.begin(), floors.end(), elevator.currentFloor);
        for (auto it = pivot; it != floors.end(); ++it) {
            elevator.moveTo(*it, true);
        }
        for (auto it = std::make_reverse_iterator(pivot); it != floors.rend(); ++it) {
            elevator.moveTo(*it, true);
        }
    }
}

void sstf(std::vector<int> floors, std::vector<Elevator>& elevators) {
    for (Elevator& elevator : elevators) {
        while (!floors.empty()) {
            auto closest = std::min_element(floors.begin(), floors.end(), [elevator](int a, int b) {
                return std::abs(a - elevator.currentFloor) < std::abs(b - elevator.currentFloor);
            });
            elevator.moveTo(*closest, true);
            floors.erase(closest);
        }
    }
}

void dynamicScheduling(std::multiset<int> requests, std::vector<Elevator>& elevators) {
    while (!requests.empty()) {
        for (Elevator& elevator : elevators) {
            if (requests.empty()) break;
            auto closest = requests.lower_bound(elevator.currentFloor);
            if (closest == requests.end()) closest = requests.begin();
            elevator.moveTo(*closest, true);
            requests.erase(closest);
        }
    }
}

std::vector<int> generateRequests(int numRequests, int maxFloor) {
    std::vector<int> requests(numRequests);
    for (int i = 0; i < numRequests; ++i) {
        requests[i] = rand() % maxFloor;
    }
    return requests;
}

int main() {
    srand(time(0));
    std::vector<int> testSizes = {10, 100, 1000, 10000, 100000};
    int maxFloor = 100;
    int numElevators = 3;
    int numSimulations = 5;
    json results;

    for (int size : testSizes) {
        std::vector<int> requests = generateRequests(size, maxFloor);

        for (int sim = 0; sim < numSimulations; ++sim) {
            std::vector<Elevator> elevators(numElevators, Elevator(rand() % maxFloor));

            fifo(requests, elevators);
            results["FIFO"][std::to_string(size)] += elevators[0].elapsedTime / numSimulations;

            elevators.assign(numElevators, Elevator(rand() % maxFloor));
            scan(requests, elevators);
            results["SCAN"][std::to_string(size)] += elevators[0].elapsedTime / numSimulations;

            elevators.assign(numElevators, Elevator(rand() % maxFloor));
            look(requests, elevators);
            results["LOOK"][std::to_string(size)] += elevators[0].elapsedTime / numSimulations;

            elevators.assign(numElevators, Elevator(rand() % maxFloor));
            sstf(requests, elevators);
            results["SSTF"][std::to_string(size)] += elevators[0].elapsedTime / numSimulations;

            std::multiset<int> dynamicRequests(requests.begin(), requests.end());
            elevators.assign(numElevators, Elevator(rand() % maxFloor));
            dynamicScheduling(dynamicRequests, elevators);
            results["DynamicScheduling"][std::to_string(size)] += elevators[0].elapsedTime / numSimulations;
        }
    }

    std::ofstream file("results/results.json");
    file << results.dump(4);
    file.close();

    std::cout << "Profiling completed. Results saved to results.json" << std::endl;
    return 0;
}
