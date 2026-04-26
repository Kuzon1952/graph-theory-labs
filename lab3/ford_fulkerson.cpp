#include "ford_fulkerson.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <queue>
#include <algorithm>
#include <vector>

static std::mt19937 rng3(std::random_device{}());

std::vector<std::vector<int>> generateCapacityMatrix(const Graph& g) {
    const int n = g.n;
    std::vector<std::vector<int>> cap(n, std::vector<int>(n, 0));
    std::uniform_int_distribution<int> dist(1, 20);
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (g.hasEdge(u, v))
                cap[u][v] = dist(rng3);
    return cap;
}

std::vector<std::vector<int>> generateCostMatrix(const Graph& g) {
    const int n = g.n;
    std::vector<std::vector<int>> cost(n, std::vector<int>(n, 0));
    std::uniform_int_distribution<int> dist(1, 10);
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (g.hasEdge(u, v))
                cost[u][v] = dist(rng3);
    return cost;
}

// BFS finds an augmenting path; prev[v] = predecessor of v on path
static bool bfs(const std::vector<std::vector<int>>& resCap, int n,
                int src, int dst, std::vector<int>& prev) {
    prev.assign(n, -1);
    prev[src] = src;
    std::queue<int> q;
    q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v = 0; v < n; v++) {
            if (prev[v] == -1 && resCap[u][v] > 0) {
                prev[v] = u;
                if (v == dst) return true;
                q.push(v);
            }
        }
    }
    return false;
}

static FFResult runFF(int n, const std::vector<std::vector<int>>& cap,
                      int src, int dst, bool verbose) {
    std::vector<std::vector<int>> resCap = cap;
    std::vector<std::vector<int>> flow(n, std::vector<int>(n, 0));
    int maxFlow = 0;
    int pathNum = 0;

    std::vector<int> prev;
    while (bfs(resCap, n, src, dst, prev)) {
        // Bottleneck
        int delta = INT_MAX;
        for (int v = dst; v != src; v = prev[v])
            delta = std::min(delta, resCap[prev[v]][v]);

        // Augment flow along path
        for (int v = dst; v != src; v = prev[v]) {
            int u = prev[v];
            resCap[u][v] -= delta;
            resCap[v][u] += delta;
            flow[u][v] += delta;
            flow[v][u] -= delta;
        }
        maxFlow += delta;
        pathNum++;

        if (verbose) {
            // Reconstruct path for display
            std::vector<int> path;
            for (int v = dst; v != src; v = prev[v]) path.push_back(v);
            path.push_back(src);
            std::reverse(path.begin(), path.end());
            std::cout << "  Path " << pathNum << ": ";
            for (int i = 0; i < (int)path.size(); i++) {
                if (i) std::cout << " -> ";
                std::cout << path[i] + 1;
            }
            std::cout << "  (delta = " << delta << ")\n";
        }
    }
    return { flow, maxFlow, src, dst };
}

FFResult fordFulkerson(int n, const std::vector<std::vector<int>>& cap,
                       int src, int dst) {
    return runFF(n, cap, src, dst, true);
}

FFResult fordFulkersonSilent(int n, const std::vector<std::vector<int>>& cap,
                              int src, int dst) {
    return runFF(n, cap, src, dst, false);
}

static void printIntMatrix(const std::vector<std::vector<int>>& M, int n,
                            const std::string& title) {
    const int CW = 6;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(CW) << M[i][j];
        std::cout << "\n";
    }
}

void printCapacityMatrix(const std::vector<std::vector<int>>& cap, int n) {
    printIntMatrix(cap, n, "CAPACITY MATRIX  Omega[i][j]:");
}

void printCostMatrix(const std::vector<std::vector<int>>& cost, int n) {
    printIntMatrix(cost, n, "COST MATRIX  D[i][j]:");
}

void printFFResult(const FFResult& res,
                   const std::vector<std::vector<int>>& cap, int n) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  FORD-FULKERSON RESULT\n";
    std::cout << "  Source: " << res.src + 1 << "   Sink: " << res.dst + 1 << "\n";
    std::cout << "  Maximum flow  phi_max = " << res.maxFlow << "\n";
    std::cout << "  " << sep << "\n";
    printCapacityMatrix(cap, n);

    // Print only arcs that carry positive flow
    std::cout << "\n  FLOW on each arc (only arcs with flow > 0):\n";
    bool any = false;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (res.flow[i][j] > 0) {
                std::cout << "    (" << i + 1 << " -> " << j + 1 << ")  "
                          << "flow=" << res.flow[i][j]
                          << "  cap=" << cap[i][j] << "\n";
                any = true;
            }
    if (!any) std::cout << "    (none)\n";
    std::cout << "\n  " << sep << "\n";
}
