#pragma once
#include "graph.h"
#include "weight_matrix.h"  // Matrix, WeightMode, generateWeightMatrix (shared)
#include <vector>


// Result bundle returned by floydWarshall()
struct FloydResult {
    std::vector<std::vector<double>> T;   // distance matrix
    std::vector<std::vector<int>>    H;   // path matrix (1-based next hop, 0 = no path)
    long long iterations;                 // always n³ (unconditionally counted)
    bool hasNegCycle;
    std::vector<int> negCycleVerts;       // 0-based indices of vertices with T[j][j]<0
};

// Runs Floyd-Warshall
//   loop order i,j,k  |  condition j≠i & T[j][i]≠INF & i≠k & T[i][k]≠INF
//   & (T[j][k]=INF or T[j][k] > T[j][i]+T[i][k])
//   iterations counted unconditionally → always n³
FloydResult floydWarshall(const Graph&                            g,
                          const std::vector<std::vector<double>>& W);

// Prints C, T, H matrices + path reconstruction + iteration count
void printFloydResult(const FloydResult&                       res,
                      const std::vector<std::vector<double>>& W,
                      int n, int src, int dst);
