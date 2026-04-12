#include "distribution.h"
#include <cmath>
#include <random>
#include <algorithm>


static std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

double normalSample(int n) {
    std::uniform_real_distribution<double> uniform(0.0, 1.0);
    auto& rng = getRng();
    double sum = 0.0;
    for (int i = 0; i < n; i++)
        sum += uniform(rng);
    // x = sqrt(12/n) * (sum - n/2)
    return std::sqrt(12.0 / n) * (sum - n / 2.0);
}

int sampleDegree(int maxDeg) {
    // Map N(0,1) to a positive integer in [1, maxDeg]
    double raw = std::abs(normalSample(12));
    // raw is half-normal; scale to [1, maxDeg]
    int d = 1 + static_cast<int>(raw * (maxDeg - 1) / 2.5);
    return std::clamp(d, 1, maxDeg);
}

std::vector<int> generateDegreeSequence(int numVertices) {//dui
    if (numVertices <= 1) return std::vector<int>(numVertices, 0);

    int target = 2 * (numVertices - 1);
    std::vector<int> degrees(numVertices);
    for (int i = 0; i < numVertices; i++)
        degrees[i] = sampleDegree(numVertices - 1);

    // Adjust sum to target by randomly incrementing/decrementing
    auto& rng = getRng();
    std::uniform_int_distribution<int> pick(0, numVertices - 1);

    int sum = 0;
    for (int d : degrees) sum += d;

    int iterations = 0;
    while (sum != target && iterations < 100000) {
        int v = pick(rng);
        if (sum > target && degrees[v] > 1) {
            degrees[v]--;
            sum--;
        } else if (sum < target && degrees[v] < numVertices - 1) {
            degrees[v]++;
            sum++;
        }
        iterations++;
    }

    return degrees;
}
