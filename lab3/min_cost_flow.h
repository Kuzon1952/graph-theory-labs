#pragma once
#include "ford_fulkerson.h"
#include <vector>
#include <string>

// ── Min Cost Flow result ──────────────────────────────────────────────────────
struct MCFResult {
    FMatrix flow;          // flow[u][v] on each arc
    double  totalFlow;     // achieved flow value
    double  totalCost;     // total cost of this flow
    long long iterations;  // Bellman-Ford iterations
};

// ── Minimum Cost Flow — teacher's algorithm ───────────────────────────────────
//
// Finds a flow of value θ = floor(2/3 * maxFlow) from src to snk
// with minimum total cost.
//
// Method (from teacher's lecture tg9a):
//   Repeat until flow value = θ:
//     1. Build modified (residual) network G~ with costs:
//        - Forward unsaturated arc (φ < c, no opposite): c̃ = c-φ, d̃ = d
//        - Saturated arc (φ = c): c̃ = 0, d̃ = ∞  (excluded)
//        - Reverse arc (φ(j,i) > 0): c̃ = φ(j,i), d̃ = -d(j,i)
//     2. Find shortest (cheapest) path src->snk using Floyd-Warshall
//        (handles negative costs from reverse arcs)
//     3. Push min(residual along path, θ - currentFlow) along path
//     4. Update flow matrix
//
MCFResult minCostFlow(const FMatrix& C,   // capacity matrix
                      const FMatrix& D,   // cost matrix
                      int n, int src, int snk,
                      double theta);      // target flow = floor(2/3 * maxFlow)
