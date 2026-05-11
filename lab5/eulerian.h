#pragma once
#include "graph.h"
#include <string>
#include <utility>
#include <vector>

struct EulerianResult {
    bool originalConnected;
    bool originalEulerian;
    bool modificationPossible;
    std::vector<int> originalDegrees;
    std::vector<int> originalOddVertices;
    std::vector<int> modifiedDegrees;
    std::vector<std::pair<int, int>> addedEdges;
    std::vector<std::pair<int, int>> removedEdges;
    std::vector<std::string> log;
    std::vector<int> eulerCycle;
};

// Checks the undirected graph, modifies only edges of a local simple graph
// when needed, and constructs an Euler cycle by Hierholzer's algorithm.
// The input Graph is not mutated.
EulerianResult buildEulerianCycle(const Graph& g);

void printEulerianResult(const EulerianResult& res);
