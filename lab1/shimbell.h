#pragma once
#include "graph.h"
#include "constants.h"
#include <vector>

using Matrix = std::vector<std::vector<double>>;

enum WeightMode { POSITIVE, NEGATIVE, MIXED };

// Generate a weight matrix for the graph's existing edges using the Normal
// distribution.  Non-edges are set to +INF (for min) and -INF (for max).
// Diagonal is always 0 (self-loop of cost 0, which lets Shimbell represent
// "at most k steps" instead of "exactly k steps").
Matrix generateWeightMatrix(const Graph& g, WeightMode mode);

// Shimbell's algorithm.
//   k == 0  -> identity matrix (diagonal 0, off-diagonal +INF / -INF)
//   k >= 1  -> multiply W by itself k times in the (min/max, +) tropical semiring
//
// Returns {minMatrix, maxMatrix}.
std::pair<Matrix, Matrix> shimbell(const Matrix& W, int k);

// Pretty-print a matrix.  Values >= INF_THRESHOLD are shown as "INF",
// values <= -INF_THRESHOLD are shown as "-INF".
void printMatrix(const Matrix& M);


