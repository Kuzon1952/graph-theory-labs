#include "weight_matrix.h"
#include <cmath>

static double sampleWeight(WeightMode mode) {
    double raw = normalSample(12) * 5.0;
    int w = static_cast<int>(std::round(raw));
    switch (mode) {
        case POSITIVE: return (w <= 0) ? 1 : w;
        case NEGATIVE: return (w >= 0) ? -1 : w;
        case MIXED:
        default:       return (w == 0) ? 1 : w;
    }
}

Matrix generateWeightMatrix(const Graph& g, WeightMode mode) {
    int n = g.n;
    Matrix W(n, std::vector<double>(n, SHIMBELL_INF));
    for (int i = 0; i < n; i++) {
        for (int j = g.directed ? 0 : i + 1; j < n; j++) {
            if (i == j) continue;
            if (g.hasEdge(i, j)) {
                double w = sampleWeight(mode);
                W[i][j] = w;
                if (!g.directed) W[j][i] = w;  // undirected edge has one weight
            }
        }
    }
    return W;
}
