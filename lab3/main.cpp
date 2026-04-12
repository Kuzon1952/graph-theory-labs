#include <iostream>
#include <limits>
#include <string>
#include "graph.h"
#include "dag_generator.h"
// #include "ford_fulkerson.h" // Lab 3 — max flow (in progress)
// #include "min_cost_flow.h"  // Lab 3 — min cost flow at 2/3*max (in progress)

static Graph gGraph;
static bool  gGraphReady = false;

static void clearInput() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

static int readInt(const std::string& prompt) {
    int v;
    while (true) {
        std::cout << prompt;
        if (std::cin >> v) { clearInput(); return v; }
        std::cout << "  Invalid input. Please enter an integer.\n";
        clearInput();
    }
}

void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }

    // Lab 3 always uses a directed graph (needed for flow algorithms)
    Graph directedGraph = generateDAG(n, true);
    gGraph = directedGraph;
    gGraphReady = true;
    gGraph.printAdjMatrix();
    std::cout << "  Directed graph with " << n << " vertices generated.\n";
}

// ── Generate capacity & cost matrices ────────────────────────────────────────
// Randomly generates two matrices from the graph's edges:
//   - Capacity matrix: how much flow each edge can carry (positive values)
//   - Cost matrix: cost per unit of flow on each edge
// Both generated using the Normal distribution (same as weight matrix in Lab 1).
// Implementation: ford_fulkerson.h (in progress)
void menuGenerateMatrices() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    std::cout << "  Generating capacity and cost matrices — in progress.\n";
    // generateFlowMatrices(gGraph);
}

// ── Maximum flow — Ford-Fulkerson ─────────────────────────────────────────────
// Finds the maximum flow from a user-chosen source to a user-chosen sink.
// Uses the Ford-Fulkerson algorithm with BFS (Edmonds-Karp variant).
// Outputs: max flow value + final flow on each edge.
// Implementation: ford_fulkerson.h / ford_fulkerson.cpp (in progress)
void menuMaxFlow() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "  Ford-Fulkerson max flow from " << src+1
              << " to " << snk+1 << " — in progress.\n";
    // fordFulkerson(gGraph, src, snk);
}

// ── Minimum cost flow ─────────────────────────────────────────────────────────
// Finds the minimum cost flow for a flow value of floor(2/3 * maxFlow).
// Uses Floyd-Warshall (from Lab 2) to find minimum cost augmenting paths.
// Outputs: total cost + flow assignment per edge.
// Implementation: min_cost_flow.h / min_cost_flow.cpp (in progress)
void menuMinCostFlow() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "  Min cost flow (2/3 * max) from " << src+1
              << " to " << snk+1 << " — in progress.\n";
    // minCostFlow(gGraph, src, snk);
}

void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY  -  Lab 3\n"
              << "========================================\n"
              << "  1. Generate directed graph\n"
              << "  2. Show graph\n"
              << "  ── Lab 3 ──\n"
              << "  3. Generate capacity & cost matrices\n"
              << "  4. Maximum flow (Ford-Fulkerson)\n"
              << "  5. Minimum cost flow (2/3 * max)\n"
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
            case 1: menuGenerateGraph();    break;
            case 2:
                if (gGraphReady) gGraph.printFull(nullptr);
                else { std::cout << "  Generate a graph first.\n"; }
                break;
            case 3: menuGenerateMatrices(); break;
            case 4: menuMaxFlow();          break;
            case 5: menuMinCostFlow();      break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
