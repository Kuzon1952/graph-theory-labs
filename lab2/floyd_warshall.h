#pragma once
#include "graph.h"
#include <vector>

// Generates a weight matrix from the graph's adjacency structure.
//   allowNegative == false : weights drawn uniformly from [1, 20]
//   allowNegative == true  : weights drawn from [-10, 20] (excluding 0)
std::vector<std::vector<double>>
generateWeightMatrix(const Graph& g, bool allowNegative);

// Runs Floyd-Warshall on the weighted graph.
// Prints:
//   - the weight matrix
//   - the full distance matrix
//   - the shortest path from src to dst (vertex sequence)
//   - total iteration count (inner loop executions)
void floydWarshall(const Graph&                          g,
                   const std::vector<std::vector<double>>& W,
                   int src, int dst);
