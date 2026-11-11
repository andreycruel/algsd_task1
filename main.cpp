#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

using namespace std;

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

int main() {
    random_device device;
    mt19937 generator(device());

    cout << "\n";
    int n = readInt("Number of cities (2..12): ", 2, 12);
    int minCost = readInt("Minimum edge cost (1..1000000): ", 1, 1000000);
    int maxCost = readInt("Maximum edge cost (min..1000000): ", minCost, 1000000);

    vector<vector<int>> matrix = createRandomMatrix(n, minCost, maxCost, generator);
    cout << "Cost matrix:\n";
    printMatrix(matrix);
    return 0;
}
