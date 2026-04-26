#pragma once
#include "graph.h"
#include <vector>

struct MSTEdge {
    int u, v;
    double weight;
};

struct BoruvkaResult {
    std::vector<MSTEdge> edges;  // MST edges (n-1 edges)
    double totalWeight;
};

// Borůvka's algorithm (lecture tg9.pdf pp.62-66):
//   While more than 1 component: for each component find the cheapest
//   outgoing edge (safe edge); add all safe edges; merge components.
//   Tie-breaking: smallest edge index (u*n+v).
//   Graph treated as undirected. Weight matrix W[i][j] gives edge weights.
BoruvkaResult boruvka(const Graph& g,
                      const std::vector<std::vector<double>>& W);

void printBoruvkaResult(const BoruvkaResult& res);
