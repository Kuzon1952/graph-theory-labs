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
generateWeightMatrix(const Graph& g, int mode) {
    static std::mt19937 rng(std::random_device{}());
    const int n = g.n;
    std::vector<std::vector<double>> W(n, std::vector<double>(n, SHIMBELL_INF));
    for (int i = 0; i < n; i++) W[i][i] = 0.0;

    std::uniform_int_distribution<int> posDist(1, 20);
    std::uniform_int_distribution<int> negDist(1, 20);
    std::uniform_int_distribution<int> coinFlip(0, 1);

    for (int u = 0; u < n; u++) {
        for (int v = 0; v < n; v++) {
            if (u == v) continue;
            if (!g.hasEdge(u, v)) continue;
            int w;
            if (mode == 1) {
                w = -negDist(rng);
            } else if (mode == 2) {
                w = posDist(rng);
                if (coinFlip(rng)) w = -w;
            } else {
                w = posDist(rng);
            }
            W[u][v] = static_cast<double>(w);
        }
    }
    return W;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Print helpers
// ─────────────────────────────────────────────────────────────────────────────
static void printDMatrix(const std::vector<std::vector<double>>& M,
                         int n, const std::string& title) {
    const int CW = 8;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++) {
            double v = M[i][j];
            if      (v >= SHIMBELL_INF / 2)  std::cout << std::setw(CW) << "INF";
            else if (v <= SHIMBELL_NINF / 2) std::cout << std::setw(CW) << "-INF";
            else    std::cout << std::setw(CW) << static_cast<long long>(v);
        }
        std::cout << "\n";
    }
}

static void printHMatrix(const std::vector<std::vector<int>>& H,
                         int n, const std::string& title) {
    const int CW = 8;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++) std::cout << std::setw(CW) << H[i][j];
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Floyd-Warshall
//
//  Loop order:  i (intermediate vertex), j (source row), k (destination col)
//  Condition:   j≠i  &  T[j][i]≠INF  &  i≠k  &  T[i][k]≠INF
//               &  (T[j][k]=INF  OR  T[j][k] > T[j][i]+T[i][k])
//  Iterations:  counted unconditionally inside innermost loop → always n³
//  H matrix:    H[i][j] = j+1 (1-based) if direct edge, 0 if none
//               updated as H[j][k] := H[j][i]  (first hop on new path)
//  Neg-cycle:   after main loop, if T[j][j] < 0 → no solution
// ─────────────────────────────────────────────────────────────────────────────
FloydResult floydWarshall(const Graph&                            g,
                          const std::vector<std::vector<double>>& W) {
    const int n = g.n;

    // ── Initialize T and H ────────────────────────────────────
    std::vector<std::vector<double>> T = W;
    std::vector<std::vector<int>>    H(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            H[i][j] = (W[i][j] < SHIMBELL_INF / 2) ? (j + 1) : 0;

    // ── Triple loop — order: i, j, k ─────────────────────────
    long long iterations = 0;
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            for (int k = 0; k < n; k++) {
                iterations++;          // unconditional — always n^3 total

                if (j == i) continue;//same ver
                if (T[j][i] >= SHIMBELL_INF / 2) continue;//no path
                if (i == k) continue;
                if (T[i][k] >= SHIMBELL_INF / 2) continue;

                double via = T[j][i] + T[i][k];
                if (T[j][k] >= SHIMBELL_INF / 2 || T[j][k] > via) {//gocha
                    H[j][k] = H[j][i];   // first hop on new shorter path
                    T[j][k] = via;
                }
            }
        }
    }

    // ── Negative-cycle detection ──────────────────────────────
    bool hasNegCycle = false;
    std::vector<int> negCycleVerts;
    for (int j = 0; j < n; j++) {
        if (T[j][j] < -1e-9) {
            hasNegCycle = true;
            negCycleVerts.push_back(j);
        }
    }

    return { T, H, iterations, hasNegCycle, negCycleVerts };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Print full result
// ─────────────────────────────────────────────────────────────────────────────
void printFloydResult(const FloydResult&                       res,
                      const std::vector<std::vector<double>>& W,
                      int n, int src, int dst) {
    const std::string sep(60, '-');

    std::cout << "\n  " << sep << "\n";
    std::cout << "  FLOYD-WARSHALL  (src=" << src + 1
              << "  dst=" << dst + 1 << ")\n";
    std::cout << "  " << sep << "\n";

    printDMatrix(W,     n, "C = WEIGHT MATRIX  (INF = no edge, 0 = self):");
    printDMatrix(res.T, n, "T = DISTANCE MATRIX  (all-pairs shortest paths):");
    printHMatrix(res.H, n, "H = PATH MATRIX  (H[i][j] = first next hop, 0 = no path):");

    // Negative cycle
    if (res.hasNegCycle) {
        //std::cout << "\n  *** NEGATIVE CYCLE DETECTED — no solution ***\n";
        //std::cout << "  Vertices on negative diagonal: ";
        for (int i = 0; i < (int)res.negCycleVerts.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << res.negCycleVerts[i] + 1;
        }
        std::cout << "\n";
        std::cout << "\n  Total iterations: " << res.iterations
                  << "  (n=" << n << "  where  n^3=" << (long long)n*n*n << ")\n";
        std::cout << "\n  " << sep << "\n";
        return;
    }

    // Path reconstruction
    //   w := src;  yield w
    //   while w != dst do  w := H[w][dst];  yield w
    std::cout << "\n  Shortest path from " << src + 1
              << " to " << dst + 1 << ":\n";

    if (src == dst) {
        std::cout << "  Path: " << src + 1 << "  (same vertex, distance = 0)\n";
    } else if (res.T[src][dst] >= SHIMBELL_INF / 2) {
        std::cout << "  No path exists.\n";
    } else {
        std::vector<int> path;
        int w = src;
        int guard = n + 2;
        bool ok = true;
        path.push_back(w);
        while (w != dst) {
            int nxt = res.H[w][dst] - 1;  // H is 1-based to convert to 0-based
            if (nxt < 0 || nxt >= n || --guard < 0) { ok = false; break; }
            w = nxt;
            path.push_back(w);
        }
        if (!ok) {
            std::cout << "  (reconstruction failed)\n";
        } else {
            std::cout << "  Path    : ";
            for (int i = 0; i < (int)path.size(); i++) {
                if (i) std::cout << " -> ";
                std::cout << path[i] + 1;
            }
            std::cout << "\n";
            std::cout << "  Distance: "
                      << static_cast<long long>(res.T[src][dst]) << "\n";
        }
    }

    std::cout << "\n  Total iterations: " << res.iterations
              << "  (n=" << n << "  where  n^3=" << (long long)n*n*n << ")\n";
    std::cout << "\n  " << sep << "\n";
}
