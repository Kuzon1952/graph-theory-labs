#include "min_cost_flow.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <climits>

// Bellman-Ford for shortest path by cost in residual network.
// Returns false if dst is unreachable.
static bool bellmanFord(const std::vector<std::vector<int>>& resCap,
                        const std::vector<std::vector<int>>& resCost,
                        int n, int src, int dst,
                        std::vector<int>& dist, std::vector<int>& prev) {
    dist.assign(n, INT_MAX);
    prev.assign(n, -1);
    dist[src] = 0;

    for (int iter = 0; iter < n - 1; iter++) {
        bool updated = false;
        for (int u = 0; u < n; u++) {
            if (dist[u] == INT_MAX) continue;
            for (int v = 0; v < n; v++) {
                if (resCap[u][v] <= 0) continue;
                if (dist[u] + resCost[u][v] < dist[v]) {
                    dist[v] = dist[u] + resCost[u][v];
                    prev[v] = u;
                    updated = true;
                }
            }
        }
        if (!updated) break;
    }
    return dist[dst] != INT_MAX;
}

MCFResult minCostFlow(int n,
                      const std::vector<std::vector<int>>& cap,
                      const std::vector<std::vector<int>>& cost,
                      int src, int dst) {
    // Step 1: find max flow (quietly)
    FFResult ff = fordFulkersonSilent(n, cap, src, dst);
    int maxFlow = ff.maxFlow;
    int theta = (2 * maxFlow) / 3;   // floor(2/3 * maxFlow)

    std::cout << "  phi_max = " << maxFlow
              << "   theta = floor(2/3 * " << maxFlow << ") = " << theta << "\n";

    if (theta == 0) {
        std::cout << "  theta = 0, nothing to route.\n";
        return { std::vector<std::vector<int>>(n, std::vector<int>(n, 0)),
                 0, 0, 0LL, maxFlow };
    }

    // Residual capacity — starts as the original cap
    std::vector<std::vector<int>> resCap = cap;
    // Residual cost — forward = cost[u][v], reverse arcs get negative cost
    std::vector<std::vector<int>> resCost = cost;
    std::vector<std::vector<int>> flow(n, std::vector<int>(n, 0));

    int totalFlow = 0;
    long long totalCost = 0;
    int pathNum = 0;

    while (totalFlow < theta) {
        std::vector<int> dist, prev;
        if (!bellmanFord(resCap, resCost, n, src, dst, dist, prev)) break;

        // Bottleneck, capped at remaining demand
        int delta = theta - totalFlow;
        for (int v = dst; v != src; v = prev[v])
            delta = std::min(delta, resCap[prev[v]][v]);

        // Augment
        for (int v = dst; v != src; v = prev[v]) {
            int u = prev[v];
            resCap[u][v] -= delta;
            resCap[v][u] += delta;
            // Reverse arc gets negated cost of the forward arc
            resCost[v][u] = -resCost[u][v];
            flow[u][v] += delta;
            flow[v][u] -= delta;
        }
        totalFlow += delta;
        totalCost += (long long)delta * dist[dst];
        pathNum++;

        // Print path
        std::vector<int> path;
        for (int v = dst; v != src; v = prev[v]) path.push_back(v);
        path.push_back(src);
        std::reverse(path.begin(), path.end());
        std::cout << "  Step " << pathNum << ": path ";
        for (int i = 0; i < (int)path.size(); i++) {
            if (i) std::cout << " -> ";
            std::cout << path[i] + 1;
        }
        std::cout << "  delta=" << delta
                  << "  path_cost=" << dist[dst]
                  << "  total_flow=" << totalFlow << "\n";
    }

    return { flow, theta, totalFlow, totalCost, maxFlow };
}

void printMCFResult(const MCFResult& res,
                    const std::vector<std::vector<int>>& cap,
                    const std::vector<std::vector<int>>& cost, int n) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  MIN-COST FLOW RESULT\n";
    std::cout << "  phi_max = " << res.maxFlow << "\n";
    std::cout << "  theta   = " << res.theta << "\n";
    std::cout << "  Flow achieved: " << res.totalFlow << "\n";
    std::cout << "  Total cost:    " << res.totalCost << "\n";
    std::cout << "  " << sep << "\n";

    // Show arcs with positive flow and their cost contribution
    std::cout << "\n  FLOW DETAILS (arcs with flow > 0):\n";
    std::cout << "    Arc       flow   cap   cost/unit   cost\n";
    std::cout << "    " << std::string(52, '-') << "\n";
    long long checkCost = 0;
    bool any = false;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (res.flow[i][j] > 0) {
                long long c = (long long)cost[i][j] * res.flow[i][j];
                checkCost += c;
                std::cout << "    (" << i + 1 << "->" << j + 1 << ")"
                          << std::setw(8) << res.flow[i][j]
                          << std::setw(6) << cap[i][j]
                          << std::setw(12) << cost[i][j]
                          << std::setw(8) << c << "\n";
                any = true;
            }
    if (!any) std::cout << "    (none)\n";
    std::cout << "    " << std::string(52, '-') << "\n";
    std::cout << "    Total cost = " << checkCost << "\n";
    std::cout << "\n  " << sep << "\n";
}
