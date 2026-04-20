#include "ford_fulkerson.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <queue>
#include <vector>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  Matrix generation
// ─────────────────────────────────────────────────────────────────────────────
FMatrix generateCapacityMatrix(const Graph& g) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> cap(1, 20);
    int n = g.n;
    FMatrix C(n, std::vector<double>(n, 0.0));
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (u != v && g.hasEdge(u, v))
                C[u][v] = cap(rng);
    return C;
}

FMatrix generateCostMatrix(const Graph& g) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<int> cost(1, 15);
    int n = g.n;
    FMatrix D(n, std::vector<double>(n, FF_INF));
    for (int i = 0; i < n; i++) D[i][i] = 0.0;
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (u != v && g.hasEdge(u, v))
                D[u][v] = cost(rng);
    return D;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Print helper
// ─────────────────────────────────────────────────────────────────────────────
void printFMatrix(const FMatrix& M, int n, const std::string& title) {
    const int CW = 8;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++) {
            double v = M[i][j];
            if (v >= FF_INF / 2)       std::cout << std::setw(CW) << "INF";
            else if (v <= -FF_INF / 2) std::cout << std::setw(CW) << "-INF";
            else                       std::cout << std::setw(CW) << static_cast<long long>(v);
        }
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Ford-Fulkerson (Edmonds-Karp: BFS to find shortest augmenting path)
//
//  Algorithm per teacher's lecture pseudocode:
//  1. Initialize flow F[u][v] = 0 for all arcs
//  2. Build residual network
//  3. BFS to find augmenting path s -> t in residual network
//  4. If path found: δ = min residual capacity along path
//     Update flow: forward arcs +δ, backward arcs -δ
//  5. Repeat until no augmenting path exists
// ─────────────────────────────────────────────────────────────────────────────
FFResult fordFulkerson(const FMatrix& C, int n, int src, int snk) {
    const std::string sep(60, '-');

    // Initialize flow matrix
    FMatrix F(n, std::vector<double>(n, 0.0));
    long long iterations = 0;

    std::cout << "\n  " << sep << "\n";
    std::cout << "  FORD-FULKERSON  (src=" << src+1 << "  snk=" << snk+1 << ")\n";
    std::cout << "  " << sep << "\n";
    printFMatrix(C, n, "CAPACITY MATRIX Ω  (0 = no arc):");

    int pathNum = 0;
    while (true) {
        // BFS to find augmenting path in residual graph
        std::vector<int> parent(n, -1);
        std::vector<bool> visited(n, false);
        std::queue<int> q;
        q.push(src);
        visited[src] = true;

        while (!q.empty() && !visited[snk]) {
            int u = q.front(); q.pop();
            for (int v = 0; v < n; v++) {
                iterations++;
                // Residual capacity: C[u][v] - F[u][v] for forward,
                //                    F[v][u] for backward (reverse arc)
                double residual = C[u][v] - F[u][v];
                if (!visited[v] && residual > 1e-9) {
                    visited[v] = true;
                    parent[v] = u;
                    q.push(v);
                }
            }
        }

        if (!visited[snk]) break;  // No augmenting path found

        // Find min residual capacity along path
        double delta = FF_INF;
        for (int v = snk; v != src; v = parent[v]) {
            int u = parent[v];
            delta = std::min(delta, C[u][v] - F[u][v]);
        }

        // Update flow along path
        pathNum++;
        std::cout << "\n  Path " << pathNum << ": ";
        std::vector<int> path;
        for (int v = snk; v != src; v = parent[v]) path.push_back(v);
        path.push_back(src);
        std::reverse(path.begin(), path.end());
        for (int i = 0; i < (int)path.size(); i++) {
            if (i) std::cout << " -> ";
            std::cout << path[i]+1;
        }
        std::cout << "   δ = " << static_cast<long long>(delta) << "\n";

        for (int v = snk; v != src; v = parent[v]) {
            int u = parent[v];
            F[u][v] += delta;
            F[v][u] -= delta;  // reverse arc
        }
    }

    // Compute total max flow (sum of flows out of source)
    double maxFlow = 0.0;
    for (int v = 0; v < n; v++) maxFlow += std::max(0.0, F[src][v]);

    printFMatrix(F, n, "FLOW MATRIX F  (result):");

    std::cout << "\n  Maximum flow: " << static_cast<long long>(maxFlow) << "\n";
    std::cout << "  Total BFS iterations: " << iterations << "\n";
    std::cout << "\n  " << sep << "\n";

    return { F, maxFlow, iterations };
}
