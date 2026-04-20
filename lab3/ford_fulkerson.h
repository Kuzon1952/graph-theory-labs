#pragma once
#include "graph.h"
#include <vector>
#include <limits>

const double FF_INF = 1e15;

// ── Matrix types ──────────────────────────────────────────────────────────────
using FMatrix = std::vector<std::vector<double>>;

// ── Generate capacity matrix Ω ────────────────────────────────────────────────
// For each existing edge (u,v): random capacity in [1, 20]
// Non-edges: 0 (no capacity)
FMatrix generateCapacityMatrix(const Graph& g);

// ── Generate cost matrix D ────────────────────────────────────────────────────
// For each existing edge (u,v): random cost in [1, 15]
// Non-edges: FF_INF (no arc)
FMatrix generateCostMatrix(const Graph& g);

// ── Print a flow/capacity matrix ──────────────────────────────────────────────
void printFMatrix(const FMatrix& M, int n, const std::string& title);

// ── Ford-Fulkerson result ─────────────────────────────────────────────────────
struct FFResult {
    FMatrix flow;          // flow[u][v] = flow on arc u->v
    double  maxFlow;       // total max flow value
    long long iterations;  // augmenting path iterations
};

// ── Ford-Fulkerson algorithm (BFS augmenting paths = Edmonds-Karp) ────────────
// Input: capacity matrix C (C[u][v] = capacity, 0 = no arc)
// Output: FFResult with flow matrix and max flow value
FFResult fordFulkerson(const FMatrix& C, int n, int src, int snk);
