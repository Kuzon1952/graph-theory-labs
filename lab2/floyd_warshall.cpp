#include "floyd_warshall.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <random>
#include <vector>
#include <algorithm>

// ─────────────────────────────────────────────────────────────────────────────
//  Weight matrix generation
// ─────────────────────────────────────────────────────────────────────────────
std::vector<std::vector<double>>
generateWeightMatrix(const Graph& g, bool allowNegative) {
    static std::mt19937 rng(std::random_device{}());

    const int n = g.n;
    // Initialize: SHIMBELL_INF for no edge, 0 on diagonal
    std::vector<std::vector<double>> W(n, std::vector<double>(n, SHIMBELL_INF));
    for (int i = 0; i < n; i++) W[i][i] = 0.0;

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (u == v) continue;
            if (!g.hasEdge(u, v)) continue;

            int w;
            if (allowNegative) {
                // [-10, -1] U [1, 20]
                std::uniform_int_distribution<int> dist(1, 30);
                w = dist(rng);
                if (w <= 10) w = -(w);          // map 1..10 -> -1..-10
                else         w = (w - 10);      // map 11..30 -> 1..20
            } else {
                std::uniform_int_distribution<int> dist(1, 20);
                w = dist(rng);
            }
            W[u][v] = static_cast<double>(w);
        }
    }
    return W;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Helper: print a distance/weight matrix
// ─────────────────────────────────────────────────────────────────────────────
static void printMatrix(const std::vector<std::vector<double>>& M,
                        int n, const std::string& title) {
    const int CW = 8;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++) {
            double v = M[i][j];
            if (v >= SHIMBELL_INF / 2)
                std::cout << std::setw(CW) << "INF";
            else if (v <= SHIMBELL_NINF / 2)
                std::cout << std::setw(CW) << "-INF";
            else
                std::cout << std::setw(CW) << static_cast<long long>(v);
        }
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Floyd-Warshall
// ─────────────────────────────────────────────────────────────────────────────
void floydWarshall(const Graph&                           g,
                   const std::vector<std::vector<double>>& W,
                   int src, int dst) {
    const std::string sep(60, '-');
    const int n = g.n;

    // ── Print weight matrix ───────────────────────────────────
    std::cout << "\n  " << sep << "\n";
    std::cout << "  FLOYD-WARSHALL  (src=" << src + 1
              << "  dst=" << dst + 1 << ")\n";
    std::cout << "  " << sep << "\n";
    printMatrix(W, n, "WEIGHT MATRIX  (INF = no edge, 0 = self):");

    // ── Initialize dist and predecessor matrices ──────────────
    std::vector<std::vector<double>> dist = W;
    // pred[i][j] = previous vertex on the shortest path from i to j
    //              -1 if no path known yet
    std::vector<std::vector<int>> pred(n, std::vector<int>(n, -1));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i != j && dist[i][j] < SHIMBELL_INF / 2)
                pred[i][j] = i;

    // ── Main loop ─────────────────────────────────────────────
    long long iterations = 0;
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            if (dist[i][k] >= SHIMBELL_INF / 2) continue; // prune
            for (int j = 0; j < n; j++) {
                iterations++;
                if (dist[k][j] >= SHIMBELL_INF / 2) continue;
                double via = dist[i][k] + dist[k][j];
                if (via < dist[i][j]) {
                    dist[i][j] = via;
                    pred[i][j] = pred[k][j];
                }
            }
        }
    }

    // ── Print distance matrix ─────────────────────────────────
    printMatrix(dist, n, "DISTANCE MATRIX  (shortest paths, all pairs):");

    // ── Reconstruct path src -> dst ───────────────────────────
    std::cout << "\n  Shortest path from " << src + 1
              << " to " << dst + 1 << ":\n";

    if (src == dst) {
        std::cout << "  Path: " << src + 1 << "  (same vertex, distance = 0)\n";
    } else if (dist[src][dst] >= SHIMBELL_INF / 2) {
        std::cout << "  No path exists between vertex "
                  << src + 1 << " and vertex " << dst + 1 << ".\n";
    } else {
        // Trace back through pred matrix
        std::vector<int> path;
        int cur = dst;
        while (cur != src && cur != -1) {
            path.push_back(cur);
            cur = pred[src][cur];
        }
        if (cur == -1) {
            std::cout << "  (path reconstruction failed — negative cycle?)\n";
        } else {
            path.push_back(src);
            std::reverse(path.begin(), path.end());

            std::cout << "  Path   : ";
            for (int i = 0; i < (int)path.size(); i++) {
                if (i) std::cout << " -> ";
                std::cout << path[i] + 1;
            }
            std::cout << "\n";
            std::cout << "  Distance: " << static_cast<long long>(dist[src][dst]) << "\n";
        }
    }

    std::cout << "\n  Total iterations   : " << iterations << "\n";
    std::cout << "\n  " << sep << "\n";
}
