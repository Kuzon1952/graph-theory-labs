#pragma once
#include <vector>

// Generate one sample from N(0,1) using the Vadzinsky formula:
//   x = sqrt(12/n) * (sum_{i=1}^{n} r_i  -  n/2)
// where r_i ~ Uniform(0,1) and n is the number of uniform samples.
double normalSample(int n = 12);

// Generate a positive integer degree from the distribution.
// Returns a value in [1, maxDeg].
int sampleDegree(int maxDeg = 6);

// Generate a degree sequence for a tree with numVertices vertices.
// A tree must satisfy: sum(degrees) == 2*(numVertices-1), all degrees >= 1.
std::vector<int> generateDegreeSequence(int numVertices);
