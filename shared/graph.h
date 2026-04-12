#pragma once
#include <vector>
#include <iostream>

class Graph {
public:
    int n;
    bool directed;
    std::vector<std::vector<int>> adj; // adj[i][j] == 1 if edge i->j exists

    Graph() : n(0), directed(false) {}
    Graph(int n, bool directed = false);

    void addEdge(int u, int v);
    bool hasEdge(int u, int v) const;

    // Returns out-neighbors (or all neighbors for undirected)
    std::vector<int> neighbors(int u) const;

    // Just the adjacency matrix (used right after generation)
    void printAdjMatrix() const;

    // Full view: adjacency list + edge list + adjacency matrix
    // weightMatrix may be nullptr — if provided it is printed as well
    void printFull(const std::vector<std::vector<double>>* weightMatrix = nullptr) const;

    bool isEmpty() const { return n == 0; }
};
