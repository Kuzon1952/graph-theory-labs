#include "boruvka.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <numeric>
#include <functional>

BoruvkaResult boruvka(const Graph& g,
                      const std::vector<std::vector<double>>& W) {
    const int n = g.n;

    // Local Union-Find
    std::vector<int> parent(n), rnk(n, 0);
    std::iota(parent.begin(), parent.end(), 0);

    std::function<int(int)> find = [&](int x) -> int {
        if (parent[x] != x) parent[x] = find(parent[x]);
        return parent[x];
    };
    auto unite = [&](int a, int b) -> bool {
        a = find(a); b = find(b);
        if (a == b) return false;
        if (rnk[a] < rnk[b]) std::swap(a, b);
        parent[b] = a;
        if (rnk[a] == rnk[b]) rnk[a]++;
        return true;
    };

    // Print current component partition
    auto printComps = [&]() {
        std::map<int, std::vector<int>> comps;
        for (int i = 0; i < n; i++) comps[find(i)].push_back(i);
        std::cout << "  Components:";
        for (auto& [root, members] : comps) {
            std::cout << " {";
            for (int k = 0; k < (int)members.size(); k++) {
                if (k) std::cout << ",";
                std::cout << members[k] + 1;
            }
            std::cout << "}";
        }
        std::cout << "\n";
    };

    std::vector<MSTEdge> mst;
    int round = 0;

    while ((int)mst.size() < n - 1) {
        round++;
        std::cout << "  --- Round " << round << " ---\n";
        printComps();

        // For each component: find cheapest outgoing edge
        std::vector<int>    bestU(n, -1), bestV(n, -1);
        std::vector<double> bestW(n, SHIMBELL_INF);

        for (int u = 0; u < n; u++) {
            for (int v = u + 1; v < n; v++) {
                bool edge = g.hasEdge(u, v) || g.hasEdge(v, u);
                if (!edge) continue;
                // For undirected, W[u][v] == W[v][u] after the weight fix.
                // Take whichever direction is valid.
                double w = (W[u][v] < SHIMBELL_INF / 2) ? W[u][v]
                         : (W[v][u] < SHIMBELL_INF / 2 ? W[v][u] : SHIMBELL_INF);
                if (w >= SHIMBELL_INF / 2) continue;

                int cu = find(u), cv = find(v);
                if (cu == cv) continue;

                // Update best for component of u
                if (w < bestW[cu] ||
                    (w == bestW[cu] && u * n + v < bestU[cu] * n + bestV[cu])) {
                    bestW[cu] = w; bestU[cu] = u; bestV[cu] = v;
                }
                // Update best for component of v
                if (w < bestW[cv] ||
                    (w == bestW[cv] && u * n + v < bestU[cv] * n + bestV[cv])) {
                    bestW[cv] = w; bestU[cv] = u; bestV[cv] = v;
                }
            }
        }

        // Add all safe edges
        bool added = false;
        for (int c = 0; c < n; c++) {
            if (bestU[c] == -1) continue;
            int u = bestU[c], v = bestV[c];
            if (unite(u, v)) {
                double w = bestW[c];
                mst.push_back({ u, v, w });
                std::cout << "    Add edge (" << u + 1 << ", " << v + 1
                          << ")  weight=" << w << "\n";
                added = true;
            }
        }
        if (!added) {
            std::cout << "  Warning: no safe edges found — graph may be disconnected.\n";
            break;
        }
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
    std::cout << "    Arc        weight\n";
    std::cout << "    " << std::string(22, '-') << "\n";
    for (auto& e : res.edges)
        std::cout << "    (" << e.u + 1 << " -> " << e.v + 1
                  << ")    " << e.weight << "\n";
    std::cout << "\n  Total MST weight = " << res.totalWeight << "\n";
    std::cout << "\n  " << sep << "\n";
}
