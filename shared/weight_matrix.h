#pragma once
#include "graph.h"
#include "constants.h"
#include "distribution.h"
#include <vector>

using Matrix = std::vector<std::vector<double>>;

enum WeightMode { POSITIVE, NEGATIVE, MIXED };

// Shared weight matrix generator used by all labs.
// Non-edges -> SHIMBELL_INF, diagonal -> SHIMBELL_INF (exact k-step semantics).
// Weights sampled from N(0,1)*5 rounded to integer, sign controlled by mode.
Matrix generateWeightMatrix(const Graph& g, WeightMode mode);
