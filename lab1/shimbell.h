#pragma once
#include "graph.h"
#include "constants.h"
#include "weight_matrix.h"  // Matrix, WeightMode, generateWeightMatrix (shared)
#include <vector>

// Shimbell's algorithm.
//   k == 0  -> identity matrix (diagonal 0, off-diagonal +INF / -INF)
//   k >= 1  -> multiply W by itself k times in the (min/max, +) tropical semiring
//
// Returns {minMatrix, maxMatrix}.
std::pair<Matrix, Matrix> shimbell(const Matrix& W, int k);

// Pretty-print a matrix.  Values >= INF_THRESHOLD are shown as "INF",
// values <= -INF_THRESHOLD are shown as "-INF".
void printMatrix(const Matrix& M);


