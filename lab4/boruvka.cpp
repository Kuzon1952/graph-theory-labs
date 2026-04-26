#include "boruvka.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <numeric>

// Union-Find with path compression
static std::vector<int> parent, rank_;

static void ufInit(int n) {
    parent.resize(n);
    rank_.assign(n, 0);
    std::iota(parent.begin(), parent.end(), 0);
}

static int ufFind(int x) {
    if (parent[x] != x) parent[x] = ufFind(parent[x]);
    return parent[x];
}

static bool ufUnion(int a, int b) {
    a = ufFind(a); b = ufFind(b);
    if (a == b) return false;
    if (rank_[a] < rank_[b]) std::swap(a, b);
    parent[b] = a;
    if (rank_[a] == rank_[b]) rank_[a]++;
    return true;
}

BoruvkaResult boruvka(const Graph& g,
                      const std::vector<std::vector<double>>& W) {
    const int n = g.n;
    ufInit(n);

    std::vector<MSTEdge> mst;
    int round = 0;

    // Repeat until we have n-1 edges (MST complete) or no progress
    while ((int)mst.size() < n - 1) {
        round++;
        std::cout << "  --- Round " << round << " ---\n";

        // For each component: best outgoing edge index (u*n+v) and its weight
        std::vector<int>    bestU(n, -1), bestV(n, -1);
        std::vector<double> bestW(n, SHIMBELL_INF);

        // Scan all undirected edges
        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                // Undirected edge exists if adj[u][v] or adj[v][u]
                bool edge = g.hasEdge(u, v) || g.hasEdge(v, u);
                if (!edge) continue;
                double w = W[u][v] < SHIMBELL_INF / 2 ? W[u][v]
                         : (W[v][u] < SHIMBELL_INF / 2 ? W[v][u] : SHIMBELL_INF);
                if (w >= SHIMBELL_INF / 2) continue;

                int cu = ufFind(u), cv = ufFind(v);
                if (cu == cv) continue;  // same component

                // Update best for component of u (tie-break by smaller u*n+v)
                if (w < bestW[cu] ||
                    (w == bestW[cu] && u * n + v < bestU[cu] * n + bestV[cu])) {
                    bestW[cu] = w;
                    bestU[cu] = u;
                    bestV[cu] = v;
                }
                // Update best for component of v
                if (w < bestW[cv] ||
                    (w == bestW[cv] && u * n + v < bestU[cv] * n + bestV[cv])) {
                    bestW[cv] = w;
                    bestU[cv] = u;
                    bestV[cv] = v;
                }
            }
        }

        // Add all safe edges
        bool added = false;
        for (int c = 0; c < n; c++) {
            if (bestU[c] == -1) continue;
            int u = bestU[c], v = bestV[c];
            if (ufUnion(u, v)) {
                double w = bestW[c];
                mst.push_back({ u, v, w });
                std::cout << "    Add edge (" << u + 1 << ", " << v + 1
                          << ")  weight=" << w << "\n";
                added = true;
            }
        }
        if (!added) break;  // graph not connected
    }

    double total = 0.0;
    for (auto& e : mst) total += e.weight;
    return { mst, total };
}

void printBoruvkaResult(const BoruvkaResult& res) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  BORUVKA MST RESULT\n";
    std::cout << "  " << sep << "\n";
    std::cout << "\n  MST edges:\n";
    for (auto& e : res.edges)
        std::cout << "    (" << e.u + 1 << ", " << e.v + 1
                  << ")  weight = " << e.weight << "\n";
    std::cout << "\n  Total MST weight = " << res.totalWeight << "\n";
    std::cout << "\n  " << sep << "\n";
}
