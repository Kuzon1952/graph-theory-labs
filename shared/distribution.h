#pragma once
#include <vector>

// Generate one sample from N(0,1) using the Vadzinsky formula:
//   x = sqrt(12/n) * (sum_{i=1}^{n} r_i  -  n/2)
// where r_i ~ Uniform(0,1) and n is the number of uniform samples.
double normalSample(int n = 12);

// Generate a positive integer degree from the distribution.
// Returns a value in [1, maxDeg].
int sampleDegree(int maxDeg = 6);

// NEW: "tree" replaced with "graph" — this function is used for both the
//      undirected graph (where sum = 2*(n-1) matches a spanning tree) and
//      the directed DAG (where degrees are scaled further by EXTRA_EDGE_SCALE).
//      The constraint sum == 2*(numVertices-1) and all degrees >= 1 still holds.
std::vector<int> generateDegreeSequence(int numVertices);
