#include <algorithm>
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

int main() {
    random_device device;
    mt19937 generator(device());

    cout << "\n";
    int n = readInt("Number of cities (2..12): ", 2, 12);
    int minCost = readInt("Minimum edge cost (1..1000000): ", 1, 1000000);
    int maxCost = readInt("Maximum edge cost (min..1000000): ", minCost, 1000000);
    int startCity = readInt("Starting city (1..n): ", 1, n) - 1;

    vector<vector<int>> matrix = createRandomMatrix(n, minCost, maxCost, generator);
    cout << "Cost matrix:\n";
    printMatrix(matrix);
    ExactResult exact = solveExactly(matrix, startCity);
    cout << "Exact algorithm (examined " << exact.examinedRoutes << " routes):\n";
    cout << "  Best:  ";
    printRoute(exact.best);
    cout << "  Worst: ";
    printRoute(exact.worst);
    return 0;
}
