#pragma once
#include "boruvka.h"
#include <vector>

struct PruferResult {
    std::vector<int>    code;     // Prüfer sequence (1-based labels, length n-2)
    std::vector<double> weights;  // weights of n-1 edges in removal order
};

// Encode MST (n vertices, n-1 edges) as Prüfer code with weights.
// Algorithm: repeatedly remove smallest leaf, record its neighbor and edge weight.
PruferResult pruferEncode(const std::vector<MSTEdge>& mstEdges, int n);

// Decode Prüfer code + weights back to a set of edges.
// Returns n-1 edges with assigned weights.
std::vector<MSTEdge> pruferDecode(const std::vector<int>&    code,
                                   const std::vector<double>& weights,
                                   int n);

void printPruferResult(const PruferResult& res, int n);
void printDecodedTree(const std::vector<MSTEdge>& edges);
