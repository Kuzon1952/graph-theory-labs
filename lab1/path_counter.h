#pragma once
#include "graph.h"
#include <vector>

// Returns all simple paths from src to dst as lists of internal (0-based) indices.
// If src == dst returns one empty path.
std::vector<std::vector<int>> findAllPaths(const Graph& g, int src, int dst);

// Convenience wrappers
bool pathExists(const Graph& g, int src, int dst);
int  countPaths (const Graph& g, int src, int dst);

// Pretty-print: "Route 1: { 1 -> 3 -> 5 }"  (1-indexed display)
void printPaths(const std::vector<std::vector<int>>& paths, int src, int dst);
