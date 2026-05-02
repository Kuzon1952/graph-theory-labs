#pragma once
#include "graph.h"
#include "boruvka.h"
#include <vector>

struct EdgeCoverResult {
    std::vector<MSTEdge> cover;   // edges in the minimum edge cover
    int matchingSize;             // size of maximum matching used
};

// Minimum edge cover via:
//   1. Find maximum matching M (Edmonds blossom, general undirected graph)
//   2. For each unmatched vertex add any incident edge
//   |min cover| = n - |M| for graphs without isolated vertices
//
// useMST: if true, work on mstEdges (a tree, which is always bipartite).
//         if false, work on the full undirected graph with weight matrix W.
EdgeCoverResult minEdgeCover(const Graph& g,
                              const std::vector<std::vector<double>>& W,
                              bool useMST,
                              const std::vector<MSTEdge>& mstEdges);

void printEdgeCoverResult(const EdgeCoverResult& res);
