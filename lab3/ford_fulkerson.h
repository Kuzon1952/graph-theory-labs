#pragma once
#include "graph.h"
#include <vector>
#include <climits>

// Capacity matrix: cap[i][j] in [1,20] for edge (i,j), else 0
std::vector<std::vector<int>> generateCapacityMatrix(const Graph& g);

// Cost matrix: D[i][j] in [1,10] for edge (i,j), else 0
std::vector<std::vector<int>> generateCostMatrix(const Graph& g);

struct FFResult {
    std::vector<std::vector<int>> flow;  // net flow on each arc
    int maxFlow;
    int src, dst;
};

// BFS-based Ford-Fulkerson (Edmonds-Karp), prints each augmenting path
FFResult fordFulkerson(int n,
                       const std::vector<std::vector<int>>& cap,
                       int src, int dst);

// Same but silent (no output), used internally by min-cost flow
FFResult fordFulkersonSilent(int n,
                              const std::vector<std::vector<int>>& cap,
                              int src, int dst);

void printCapacityMatrix(const std::vector<std::vector<int>>& cap, int n);
void printCostMatrix(const std::vector<std::vector<int>>& cost, int n);
void printFFResult(const FFResult& res,
                   const std::vector<std::vector<int>>& cap, int n);
