#include <iostream>
#include <limits>
#include <string>
#include <cmath>
#include "graph.h"
#include "dag_generator.h"
#include "ford_fulkerson.h"
#include "min_cost_flow.h"

static Graph   gGraph;
static bool    gGraphReady = false;
static FMatrix gCapacity;
static FMatrix gCostMatrix;
static bool    gFlowMatricesReady = false;
static FFResult gFFResult;
static bool    gFFDone = false;

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
static int readInt(const std::string& prompt) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { clearInput(); return v; }
        std::cout << "  Invalid input.\n";
        clearInput();
    }
}

void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }
    std::cout << "  Graph type:\n    1. Directed\n    2. Undirected\n";
    bool directed = (readInt("  Choose [1/2]: ") == 1);
    gGraph = generateDAG(n, directed);
    gGraphReady = true;
    gFlowMatricesReady = false;
    gFFDone = false;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated.\n";
}

void menuGenerateFlowMatrices() {
    if (!gGraphReady) { std::cout << "  Generate a graph first.\n"; return; }
    gCapacity   = generateCapacityMatrix(gGraph);
    gCostMatrix = generateCostMatrix(gGraph);
    gFlowMatricesReady = true;
    gFFDone = false;
    printFMatrix(gCapacity,   gGraph.n, "CAPACITY MATRIX Ω  (0 = no arc):");
    printFMatrix(gCostMatrix, gGraph.n, "COST MATRIX D  (INF = no arc):");
}

void menuMaxFlow() {
    if (!gGraphReady || !gFlowMatricesReady) {
        std::cout << "  Generate graph and matrices first.\n"; return;
    }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n || src == snk) {
        std::cout << "  Invalid vertices.\n"; return;
    }
    gFFResult = fordFulkerson(gCapacity, gGraph.n, src, snk);
    gFFDone = true;
    long long theta = static_cast<long long>(std::floor(2.0/3.0 * gFFResult.maxFlow));
    std::cout << "\n  φ_max = " << static_cast<long long>(gFFResult.maxFlow)
              << "   →   θ = ⌊2/3 × φ_max⌋ = " << theta << "\n";
}

void menuMinCostFlow() {
    if (!gGraphReady || !gFlowMatricesReady || !gFFDone) {
        std::cout << "  Complete steps 1, 2, 3 first.\n"; return;
    }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n || src == snk) {
        std::cout << "  Invalid vertices.\n"; return;
    }
    double theta = std::floor(2.0/3.0 * gFFResult.maxFlow);
    if (theta < 1.0) { std::cout << "  θ = 0 — no flow needed.\n"; return; }
    minCostFlow(gCapacity, gCostMatrix, gGraph.n, src, snk, theta);
}

void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY  -  Lab 3\n"
              << "========================================\n"
              << "  1. Generate graph\n"
              << "  2. Generate capacity & cost matrices\n"
              << "  3. Maximum flow (Ford-Fulkerson)\n"
              << "  4. Minimum cost flow (θ = ⌊2/3 × max⌋)\n"
              << "  0. Exit\n"
              << "----------------------------------------\n";
}

int main() {
    int choice = -1;
    while (choice != 0) {
        printMenu();
        choice = readInt("  Enter choice: ");
        std::cout << "\n";
        switch (choice) {
            case 1: menuGenerateGraph();       break;
            case 2: menuGenerateFlowMatrices();break;
            case 3: menuMaxFlow();             break;
            case 4: menuMinCostFlow();         break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
