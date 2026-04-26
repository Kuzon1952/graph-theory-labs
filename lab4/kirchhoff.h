#pragma once
#include "graph.h"

// Kirchhoff's Matrix-Tree Theorem:
//   Build Laplacian L where L[i][i]=degree(i), L[i][j]=-1 if edge(i,j)
//   Remove any row k and column k; the determinant of the remaining
//   (n-1)x(n-1) matrix equals the number of spanning trees.
//   (Graph treated as undirected regardless of directed flag.)
long long countSpanningTrees(const Graph& g);

void printKirchhoffResult(const Graph& g, long long count);
