#pragma once
#include "graph.h"
#include <string>
#include <utility>
#include <vector>

struct EulerianResult {
    bool originalConnected;
    bool originalEulerian;
    std::vector<int> originalDegrees;
    std::vector<int> originalOddVertices;
    std::vector<int> modifiedDegrees;
    std::vector<std::pair<int, int>> addedEdges;
    std::vector<std::string> log;
    std::vector<int> eulerCycle;
};

// Checks the undirected graph, modifies a local multigraph by adding/duplicating
// edges when needed, and constructs an Euler cycle by Hierholzer's algorithm.
// The input Graph is not mutated.
EulerianResult buildEulerianCycle(const Graph& g);

void printEulerianResult(const EulerianResult& res);
