#include <algorithm>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

struct RouteResult {
    vector<int> route;
    long long cost = 0;
};

struct ExactResult {
    RouteResult best;
    RouteResult worst;
    long long examinedRoutes = 0;
};

void printMatrix(const vector<vector<int>>& matrix) {
    int n = static_cast<int>(matrix.size());
    cout << "     ";
    for (int j = 0; j < n; ++j) {
        cout << setw(5) << j + 1;
    }
    cout << '\n';

    for (int i = 0; i < n; ++i) {
        cout << setw(3) << i + 1 << "  ";
        for (int j = 0; j < n; ++j) {
            cout << setw(5) << matrix[i][j];
        }
        cout << '\n';
    }
}

vector<vector<int>> createRandomMatrix(int n, int minCost, int maxCost,
                                       mt19937& generator) {
    vector<vector<int>> matrix(n, vector<int>(n, 0));
    uniform_int_distribution<int> distribution(minCost, maxCost);

    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                matrix[i][j] = distribution(generator);
            }
        }
    }
    return matrix;
}

int readInt(const string& message, int minimum, int maximum) {
    int value;
    while (true) {
        cout << message;
        if (cin >> value && value >= minimum && value <= maximum) {
            return value;
        }
        cout << "Incorrect value. Try again.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void printRoute(const RouteResult& result) {
    for (size_t i = 0; i < result.route.size(); ++i) {
        cout << result.route[i] + 1 << " -> ";
    }
    cout << result.route.front() + 1 << " | cost = " << result.cost << '\n';
}

long long calculateRouteCost(const vector<int>& route,
                             const vector<vector<int>>& matrix) {
    long long cost = 0;
    for (size_t i = 0; i + 1 < route.size(); ++i) {
        cost += matrix[route[i]][route[i + 1]];
    }
    cost += matrix[route.back()][route.front()];
    return cost;
}

ExactResult solveExactly(const vector<vector<int>>& matrix, int startCity) {
    int n = static_cast<int>(matrix.size());
    vector<int> otherCities;
    for (int city = 0; city < n; ++city) {
        if (city != startCity) {
            otherCities.push_back(city);
        }
    }

    ExactResult answer;
    answer.best.cost = numeric_limits<long long>::max();
    answer.worst.cost = numeric_limits<long long>::min();

    do {
        vector<int> currentRoute;
        currentRoute.push_back(startCity);
        currentRoute.insert(currentRoute.end(), otherCities.begin(), otherCities.end());
        long long currentCost = calculateRouteCost(currentRoute, matrix);
        ++answer.examinedRoutes;

        if (currentCost < answer.best.cost) {
            answer.best = {currentRoute, currentCost};
        }
        if (currentCost > answer.worst.cost) {
            answer.worst = {currentRoute, currentCost};
        }
    } while (next_permutation(otherCities.begin(), otherCities.end()));

    return answer;
}

RouteResult solveByNearestNeighbour(const vector<vector<int>>& matrix,
                                    int startCity) {
    int n = static_cast<int>(matrix.size());
    vector<bool> visited(n, false);
    vector<int> route;
    int currentCity = startCity;

    visited[currentCity] = true;
    route.push_back(currentCity);

    for (int step = 1; step < n; ++step) {
        int nextCity = -1;
        int smallestCost = numeric_limits<int>::max();

        for (int candidate = 0; candidate < n; ++candidate) {
            if (!visited[candidate] &&
                matrix[currentCity][candidate] < smallestCost) {
                smallestCost = matrix[currentCity][candidate];
                nextCity = candidate;
            }
        }

        visited[nextCity] = true;
        route.push_back(nextCity);
        currentCity = nextCity;
    }
    return {route, calculateRouteCost(route, matrix)};
}

double heuristicQuality(const ExactResult& exact, const RouteResult& heuristic) {
    long long range = exact.worst.cost - exact.best.cost;
    if (range == 0) {
        return 100.0;
    }
    return 100.0 * static_cast<double>(exact.worst.cost - heuristic.cost) / range;
}

void runOneComparison(const vector<vector<int>>& matrix, int startCity,
                      bool showMatrix) {
    if (showMatrix) {
        cout << "Cost matrix:\n";
        printMatrix(matrix);
    }

    auto exactStart = chrono::high_resolution_clock::now();
    ExactResult exact = solveExactly(matrix, startCity);
    auto exactFinish = chrono::high_resolution_clock::now();

    auto heuristicStart = chrono::high_resolution_clock::now();
    RouteResult heuristic = solveByNearestNeighbour(matrix, startCity);
    auto heuristicFinish = chrono::high_resolution_clock::now();

    auto exactTime = chrono::duration_cast<chrono::microseconds>(exactFinish - exactStart);
    auto heuristicTime = chrono::duration_cast<chrono::microseconds>(heuristicFinish - heuristicStart);

    cout << "Exact algorithm (examined " << exact.examinedRoutes << " routes):\n";
    cout << "  Best:  ";
    printRoute(exact.best);
    cout << "  Worst: ";
    printRoute(exact.worst);
    cout << "  Time: " << exactTime.count() << " microseconds\n";
    cout << "Nearest-neighbour heuristic:\n";
    cout << "  Route: ";
    printRoute(heuristic);
    cout << "  Time: " << heuristicTime.count() << " microseconds\n";
    cout << fixed << setprecision(2);
    cout << "  Quality: " << heuristicQuality(exact, heuristic) << "%\n";
}

int main() {
    random_device device;
    mt19937 generator(device());

    cout << "\n";
    int n = readInt("Number of cities (2..12): ", 2, 12);
    int minCost = readInt("Minimum edge cost (1..1000000): ", 1, 1000000);
    int maxCost = readInt("Maximum edge cost (min..1000000): ", minCost, 1000000);
    int startCity = readInt("Starting city (1..n): ", 1, n) - 1;

    vector<vector<int>> matrix = createRandomMatrix(n, minCost, maxCost, generator);
    runOneComparison(matrix, startCity, true);
    return 0;
}
