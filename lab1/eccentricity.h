#pragma once
#include "graph.h"
#include <vector>

// BFS from src following directed (or undirected) edges.
// Returns dist[v] = shortest-path distance, or -1 if unreachable.
std::vector<int> bfs(const Graph& g, int src);

struct GraphMetrics {
    std::vector<int> eccentricities;        // ecc[v]; 0 if no other vertex is reachable
    std::vector<std::vector<int>> distMatrix; // distMatrix[u][v] = BFS distance, -1 if unreachable
    int radius;                              // min eccentricity among vertices with ecc > 0
    int diameter;                            // max eccentricity
    std::vector<int> center;                 // vertices achieving the radius
    std::vector<int> diametralVertices;      // vertices achieving the diameter
};

GraphMetrics computeMetrics(const Graph& g);
void printMetrics(const GraphMetrics& m);
