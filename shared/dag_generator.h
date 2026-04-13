#pragma once
#include "graph.h"

// renamed from generateTree() to generateDAG() because the result is not
// a tree — a tree has exactly n-1 edges but our directed graph generates up to
// n*(n-1)/2 edges. The correct term is DAG (Directed Acyclic Graph).
// If directed == false, returns an undirected connected acyclic graph derived
// by symmetrizing the directed DAG (ignoring edge directions).
Graph generateDAG(int n, bool directed = false);
