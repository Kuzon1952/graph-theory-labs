#include "min_cost_flow.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <cmath>

// ─────────────────────────────────────────────────────────────────────────────
//  Bellman-Ford shortest path (handles negative weights)
//  Returns dist[] and pred[] arrays. dist[v] = shortest cost from src to v.
//  Returns false if negative cycle detected.
// ─────────────────────────────────────────────────────────────────────────────
static bool bellmanFord(const FMatrix& costRes,   // residual cost matrix
                        const FMatrix& capRes,    // residual capacity matrix
                        int n, int src,
                        std::vector<double>& dist,
                        std::vector<int>&    pred,
                        long long& iters) {
    dist.assign(n, FF_INF);
    pred.assign(n, -1);
    dist[src] = 0.0;

    for (int iter = 0; iter < n - 1; iter++) {
        bool updated = false;
        for (int u = 0; u < n; u++) {
            if (dist[u] >= FF_INF / 2) continue;
            for (int v = 0; v < n; v++) {
                iters++;
                if (capRes[u][v] < 1e-9)  continue;   // no residual capacity
                if (costRes[u][v] >= FF_INF / 2) continue; // no arc
                double nd = dist[u] + costRes[u][v];
                if (nd < dist[v] - 1e-9) {
                    dist[v] = nd;
                    pred[v] = u;
                    updated = true;
                }
            }
        }
        if (!updated) break;
    }

    // Check for negative cycles
    for (int u = 0; u < n; u++) {
        if (dist[u] >= FF_INF / 2) continue;
        for (int v = 0; v < n; v++) {
            if (capRes[u][v] < 1e-9) continue;
            if (costRes[u][v] >= FF_INF / 2) continue;
            if (dist[u] + costRes[u][v] < dist[v] - 1e-9)
                return false; // negative cycle
        }
    }
    return true;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Print helper (reuse from ford_fulkerson)
// ─────────────────────────────────────────────────────────────────────────────
static void printM(const FMatrix& M, int n, const std::string& title) {
    const int CW = 8;
    std::cout << "\n  " << title << "\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j+1;
    std::cout << "\n  " << std::string(7 + n*CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i+1 << " |";
        for (int j = 0; j < n; j++) {
            double v = M[i][j];
            if      (v >= FF_INF/2)  std::cout << std::setw(CW) << "INF";
            else if (v <= -FF_INF/2) std::cout << std::setw(CW) << "-INF";
            else                     std::cout << std::setw(CW) << static_cast<long long>(v);
        }
        std::cout << "\n";
    }
}

// ─────────────────────────────────────────────────────────────────────────────
//  Min Cost Flow — teacher's algorithm from lecture tg9a
//
//  Network modification rules (from lecture slides p.3-4):
//  For each arc (i,j) in original network:
//    If φ(i,j) > 0:  add reverse arc (j,i): c̃(j,i)=φ(i,j), d̃(j,i)=-d(i,j)
//    If φ(i,j) < c(i,j) AND φ(j,i)=0 (unsaturated, no opposite flow):
//      keep arc (i,j): c̃(i,j)=c(i,j)-φ(i,j), d̃(i,j)=d(i,j)
//    If φ(i,j) = c(i,j) AND φ(j,i)=0 (saturated):
//      c̃(i,j)=0, d̃(i,j)=∞  (arc effectively removed)
// ─────────────────────────────────────────────────────────────────────────────
MCFResult minCostFlow(const FMatrix& C, const FMatrix& D,
                      int n, int src, int snk, double theta) {
    const std::string sep(60, '-');

    std::cout << "\n  " << sep << "\n";
    std::cout << "  MIN COST FLOW  (src=" << src+1 << "  snk=" << snk+1
              << "  θ=" << static_cast<long long>(theta) << ")\n";
    std::cout << "  " << sep << "\n";
    printM(C, n, "CAPACITY MATRIX Ω:");
    printM(D, n, "COST MATRIX D:");

    // Current flow on each arc
    FMatrix F(n, std::vector<double>(n, 0.0));
    double  currentFlow = 0.0;
    double  totalCost   = 0.0;
    long long iters     = 0;
    int pathNum = 0;

    while (currentFlow < theta - 1e-9) {
        // ── Build residual network ────────────────────────────────────────────
        // capRes[u][v]  = residual capacity
        // costRes[u][v] = cost on residual arc
        FMatrix capRes (n, std::vector<double>(n, 0.0));
        FMatrix costRes(n, std::vector<double>(n, FF_INF));

        for (int u = 0; u < n; u++) {
            for (int v = 0; v < n; v++) {
                if (u == v) continue;

                // Forward arc (u,v) exists in original network
                if (C[u][v] > 1e-9) {
                    double residCap = C[u][v] - F[u][v];
                    if (residCap > 1e-9 && F[v][u] < 1e-9) {
                        // Unsaturated, no opposite flow → rule 4 of lecture
                        capRes[u][v]  = residCap;
                        costRes[u][v] = D[u][v];
                    }
                    // Saturated: capRes stays 0 (rule 5)
                }

                // Reverse arc (v,u): add if F[v][u] > 0 → rule 3 of lecture
                // i.e., if there's flow on arc (v,u) in original
                if (F[v][u] > 1e-9) {
                    capRes[u][v]  += F[v][u];
                    costRes[u][v]  = -D[v][u];  // negative cost (save money)
                }
            }
        }

        // ── Find shortest path using Bellman-Ford ─────────────────────────────
        std::vector<double> dist;
        std::vector<int>    pred;
        bool ok = bellmanFord(costRes, capRes, n, src, dist, pred, iters);
        if (!ok) {
            std::cout << "\n  *** Negative cycle detected — stopping.\n";
            break;
        }
        if (dist[snk] >= FF_INF / 2) {
            std::cout << "\n  No path from " << src+1 << " to " << snk+1
                      << " in residual network — flow " << currentFlow
                      << " is maximum achievable.\n";
            break;
        }

        // ── Reconstruct path ──────────────────────────────────────────────────
        std::vector<int> path;
        for (int v = snk; v != src; v = pred[v]) path.push_back(v);
        path.push_back(src);
        std::reverse(path.begin(), path.end());

        // ── Find max flow along path (limited by θ - currentFlow) ─────────────
        double pushable = theta - currentFlow;
        for (int i = 0; i + 1 < (int)path.size(); i++) {
            int u = path[i], v = path[i+1];
            pushable = std::min(pushable, capRes[u][v]);
        }
        if (pushable < 1e-9) break;

        pathNum++;
        std::cout << "\n  Path " << pathNum << ": ";
        for (int i = 0; i < (int)path.size(); i++) {
            if (i) std::cout << " -> ";
            std::cout << path[i]+1;
        }
        std::cout << "   cost=" << static_cast<long long>(dist[snk])
                  << "   push=" << static_cast<long long>(pushable) << "\n";

        // ── Update flow ───────────────────────────────────────────────────────
        for (int i = 0; i + 1 < (int)path.size(); i++) {
            int u = path[i], v = path[i+1];
            // Determine if this is a forward or reverse arc
            if (C[u][v] > 1e-9 && F[v][u] < 1e-9) {
                // Forward arc in original network
                F[u][v] += pushable;
            } else if (F[v][u] > 1e-9) {
                // Reverse arc: reduce flow on (v,u)
                F[v][u] -= pushable;
            }
        }

        currentFlow += pushable;
        totalCost   += pushable * dist[snk];
    }

    // ── Final output ──────────────────────────────────────────────────────────
    std::cout << "\n  " << sep << "\n";
    printM(F, n, "FINAL FLOW MATRIX:");

    // Compute actual total cost from flow matrix and cost matrix
    totalCost = 0.0;
    std::cout << "\n  Flow breakdown:\n";
    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (F[u][v] > 1e-9 && D[u][v] < FF_INF/2) {
                double arcCost = F[u][v] * D[u][v];
                totalCost += arcCost;
                std::cout << "    arc " << u+1 << "->" << v+1
                          << ":  flow=" << static_cast<long long>(F[u][v])
                          << "  cost/unit=" << static_cast<long long>(D[u][v])
                          << "  subtotal=" << static_cast<long long>(arcCost) << "\n";
            }

    std::cout << "\n  Achieved flow : " << static_cast<long long>(currentFlow)
              << "  (θ = " << static_cast<long long>(theta) << ")\n";
    std::cout << "  Total cost    : " << static_cast<long long>(totalCost) << "\n";
    std::cout << "  BF iterations : " << iters << "\n";
    std::cout << "\n  " << sep << "\n";

    return { F, currentFlow, totalCost, iters };
}
