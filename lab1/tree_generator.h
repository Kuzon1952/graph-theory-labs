#pragma once
#include "graph.h"

// Generate a random connected acyclic graph (tree) with n vertices.
// Vertex degrees are biased by a Normal-distribution-generated degree sequence
// (Vadzinsky formula). Edges are added by degree-proportional random attachment.
//
// If directed == true, generates a rooted arborescence: edges flow parent -> child.
Graph generateTree(int n, bool directed = false);
