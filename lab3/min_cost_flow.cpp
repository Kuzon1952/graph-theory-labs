#include "min_cost_flow.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <climits>

// ─────────────────────────────────────────────────────────────────────────────
//  Floyd-Warshall on the residual cost graph.
//
//  Used at every iteration of the successive-shortest-paths algorithm
//  (lab task: "use the previously implemented Floyd-Warshall").
//
//  Builds T (distance) and H (next-hop, 1-based) matrices from:
//    resCap[u][v] > 0  →  arc exists with cost resCost[u][v]
//    resCap[u][v] == 0 →  arc absent
//
//  Loop order: i (intermediate), j (source row), k (destination col).
//  H[j][k] = first hop from j toward k (1-based vertex index).
// ─────────────────────────────────────────────────────────────────────────────
struct FWCostResult {
    std::vector<std::vector<int>> T;   // shortest cost distances
    std::vector<std::vector<int>> H;   // next-hop matrix (1-based, 0 = no path)
};

static const int FW_INF = INT_MAX / 2;

static FWCostResult floydWarshallCost(
        const std::vector<std::vector<int>>& resCap,
        const std::vector<std::vector<int>>& resCost,
        int n) {

    std::vector<std::vector<int>> T(n, std::vector<int>(n, FW_INF));
    std::vector<std::vector<int>> H(n, std::vector<int>(n, 0));

    // Initialise: diagonal = 0, edges = cost where capacity > 0
    for (int i = 0; i < n; i++) {
        T[i][i] = 0;
        for (int j = 0; j < n; j++) {
            if (i != j && resCap[i][j] > 0) {
                T[i][j] = resCost[i][j];
                H[i][j] = j + 1;          // direct edge → first hop is j
            }
        }
    }

    // Triple loop — same order as Lab 2: i (intermediate), j (row), k (col)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (j == i || T[j][i] == FW_INF) continue;
            for (int k = 0; k < n; k++) {
                if (i == k || T[i][k] == FW_INF) continue;
                int via = T[j][i] + T[i][k];
                if (T[j][k] == FW_INF || T[j][k] > via) {
                    T[j][k] = via;
                    H[j][k] = H[j][i];    // first hop toward k is same as toward i
                }
            }
        }
    }

    return { T, H };
}

// Reconstruct path from src to dst using Floyd-Warshall next-hop matrix H.
// Returns empty vector if no path.
static std::vector<int> reconstructPath(
        const std::vector<std::vector<int>>& H,
        int src, int dst, int n) {

    if (H[src][dst] == 0) return {};
    std::vector<int> path;
    int w = src;
    path.push_back(w);
    int guard = n + 2;
    while (w != dst) {
        int nxt = H[w][dst] - 1;          // convert 1-based → 0-based
        if (nxt < 0 || nxt >= n || --guard < 0) return {};
        w = nxt;
        path.push_back(w);
    }
    return path;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Min-cost flow  —  successive shortest paths via Floyd-Warshall
//
//  Algorithm (lecture tg9a):
//   1. phi_max = Ford-Fulkerson(cap, src, dst).
//      theta = floor(2/3 * phi_max).
//   2. Build residual network (initially same as cap/cost).
//   3. While total_flow < theta:
//      a. Run Floyd-Warshall on residual cost graph.
//      b. Get shortest path src→dst from H matrix.
//      c. delta = min(residual caps on path, theta - total_flow).
//      d. Augment flow by delta along path.
//      e. Update residual network (lecture page 4 rules):
//           - Forward arc (u,v): resCap[u][v] -= delta
//           - Reverse arc (v,u): resCap[v][u] += delta,
//                                resCost[v][u] = -d(u,v)  (negated original cost)
// ─────────────────────────────────────────────────────────────────────────────
MCFResult minCostFlow(int n,
                      const std::vector<std::vector<int>>& cap,
                      const std::vector<std::vector<int>>& cost,
                      int src, int dst) {

    // Step 1: max flow
    FFResult ff = fordFulkersonSilent(n, cap, src, dst);
    int maxFlow = ff.maxFlow;
    int theta   = (2 * maxFlow) / 3;

    std::cout << "  phi_max = " << maxFlow
              << "   theta = floor(2/3 * " << maxFlow << ") = " << theta << "\n";

    if (maxFlow == 0) {
        std::cout << "  No path from source to sink — max flow is 0.\n";
        return { std::vector<std::vector<int>>(n, std::vector<int>(n,0)),
                 0, 0, 0LL, 0 };
    }
    if (theta == 0) {
        std::cout << "  theta = 0, nothing to route.\n";
        return { std::vector<std::vector<int>>(n, std::vector<int>(n,0)),
                 0, 0, 0LL, maxFlow };
    }

    // Step 2: residual network starts as the original capacity / cost matrices
    std::vector<std::vector<int>> resCap  = cap;
    std::vector<std::vector<int>> resCost = cost;
    std::vector<std::vector<int>> flow(n, std::vector<int>(n, 0));

    int       totalFlow = 0;
    long long totalCost = 0;
    int       pathNum   = 0;

    // Step 3: successive shortest paths
    while (totalFlow < theta) {

        // 3a. Floyd-Warshall on current residual cost graph
        FWCostResult fw = floydWarshallCost(resCap, resCost, n);

        // 3b. shortest path src→dst
        if (fw.T[src][dst] == FW_INF) break;   // no path in residual graph
        std::vector<int> path = reconstructPath(fw.H, src, dst, n);
        if (path.empty()) break;

        // 3c. bottleneck = min(residual cap on path, remaining demand)
        int delta = theta - totalFlow;
        for (int i = 0; i + 1 < (int)path.size(); i++)
            delta = std::min(delta, resCap[path[i]][path[i+1]]);

        // 3d-e. augment and update residual network
        for (int i = 0; i + 1 < (int)path.size(); i++) {
            int u = path[i], v = path[i+1];
            int usedCost = resCost[u][v];
            resCap[u][v]  -= delta;              // forward arc: reduce capacity
            resCap[v][u]  += delta;              // reverse arc: add capacity
            resCost[v][u]  = -usedCost;          // reverse arc cancels traversed edge
            flow[u][v]    += delta;
            flow[v][u]    -= delta;
        }
        totalFlow += delta;
        totalCost += (long long)delta * fw.T[src][dst];
        pathNum++;

        // Print this step
        std::cout << "  Step " << pathNum << " [Floyd-Warshall]: ";
        for (int i = 0; i < (int)path.size(); i++) {
            if (i) std::cout << " -> ";
            std::cout << path[i] + 1;
        }
        std::cout << "  delta=" << delta
                  << "  path_cost=" << fw.T[src][dst]
                  << "  total_flow=" << totalFlow << "\n";
    }

    return { flow, theta, totalFlow, totalCost, maxFlow };
}

// ─────────────────────────────────────────────────────────────────────────────
//  Print result
// ─────────────────────────────────────────────────────────────────────────────
void printMCFResult(const MCFResult& res,
                    const std::vector<std::vector<int>>& cap,
                    const std::vector<std::vector<int>>& cost, int n) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  MIN-COST FLOW RESULT\n";
    std::cout << "  phi_max        = " << res.maxFlow   << "\n";
    std::cout << "  theta (2/3*max)= " << res.theta     << "\n";
    std::cout << "  Flow achieved  = " << res.totalFlow << "\n";
    std::cout << "  Total cost     = " << res.totalCost << "\n";
    std::cout << "  " << sep << "\n";

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
                std::cout << "    (" << i+1 << "->" << j+1 << ")"
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
