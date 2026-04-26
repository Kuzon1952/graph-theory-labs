#pragma once
#include "ford_fulkerson.h"
#include <vector>

struct MCFResult {
    std::vector<std::vector<int>> flow;  // net flow on each arc
    int theta;                           // target flow = floor(2/3 * maxFlow)
    int totalFlow;                       // flow achieved (== theta if feasible)
    long long totalCost;
    int maxFlow;
};

// Successive shortest paths (Bellman-Ford) to find min-cost flow of theta units
// theta = floor(2/3 * maxFlow)
MCFResult minCostFlow(int n,
                      const std::vector<std::vector<int>>& cap,
                      const std::vector<std::vector<int>>& cost,
                      int src, int dst);

void printMCFResult(const MCFResult& res,
                    const std::vector<std::vector<int>>& cap,
                    const std::vector<std::vector<int>>& cost, int n);
