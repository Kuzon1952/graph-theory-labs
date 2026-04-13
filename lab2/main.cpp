#include <iostream>
#include <limits>
#include <string>
#include "graph.h"
#include "dag_generator.h"
#include "dfs.h"
#include "floyd_warshall.h"

// ── Global state ──────────────────────────────────────────────────────────────
static Graph                          gGraph;
static bool                           gGraphReady  = false;
static std::vector<std::vector<double>> gWeightMatrix;
static bool                           gWeightReady = false;

// ── Input helpers ─────────────────────────────────────────────────────────────
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

// ── Menu handlers ─────────────────────────────────────────────────────────────

// Option 1: Generate graph (from Lab 1)
void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }

    std::cout << "  Graph type:\n"
              << "    1. Directed\n"
              << "    2. Undirected\n";
    int choice = readInt("  Choose [1/2]: ");
    bool directed = (choice == 1);

    gGraph       = generateDAG(n, directed);
    gGraphReady  = true;
    gWeightReady = false;   // new graph → old weights are stale

    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated ("
              << (directed ? "directed" : "undirected") << ").\n";
}

// Option 2: Show graph (adjacency list + edge list + matrix + weights if ready)
void menuShowGraph() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    gGraph.printFull(gWeightReady ? &gWeightMatrix : nullptr);
}

// ── DFS traversal ─────────────────────────────────────────────────────────────
// Visits all vertices reachable from a user-chosen start vertex using
// iterative DFS. Outputs traversal order and total iteration count.
void menuDFS() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }

    int start = readInt("  Enter start vertex (1-based): ") - 1;
    if (start < 0 || start >= gGraph.n) {
        std::cout << "  Vertex out of range (1 .. " << gGraph.n << ").\n";
        return;
    }

    dfs(gGraph, start);
}

// ── Floyd-Warshall shortest path ──────────────────────────────────────────────
// Step 1: generate (or reuse) weight matrix — user chooses positive/negative.
// Step 2: run Floyd-Warshall.
// Outputs: weight matrix, full distance matrix, path sequence, iteration count.
void menuFloydWarshall() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }

    // Ask whether to reuse an existing weight matrix
    if (gWeightReady) {
        std::cout << "  A weight matrix already exists for this graph.\n"
                  << "    1. Reuse it\n"
                  << "    2. Generate a new one\n";
        int reuse = readInt("  Choose [1/2]: ");
        if (reuse != 1) gWeightReady = false;
    }

    if (!gWeightReady) {
        std::cout << "  Weight mode:\n"
                  << "    1. Positive only  [1 .. 20]\n"
                  << "    2. With negative weights  [-10 .. 20]\n";
        int mode = readInt("  Choose [1/2]: ");
        bool allowNegative = (mode == 2);
        gWeightMatrix = generateWeightMatrix(gGraph, allowNegative);
        gWeightReady  = true;
        std::cout << "  Weight matrix generated"
                  << (allowNegative ? " (with negative weights)" : " (positive only)")
                  << ".\n";
    }

    int src = readInt("  Source vertex (1-based): ") - 1;
    int dst = readInt("  Destination vertex (1-based): ") - 1;

    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex out of range (1 .. " << gGraph.n << ").\n";
        return;
    }

    floydWarshall(gGraph, gWeightMatrix, src, dst);
}

// ── Algorithm comparison ──────────────────────────────────────────────────────
// Runs DFS (from vertex 1) and Floyd-Warshall (vertex 1 -> last vertex)
// on the same graph and prints both iteration counts side-by-side.
void menuCompare() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }

    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  ALGORITHM COMPARISON\n";
    std::cout << "  " << sep << "\n\n";
    std::cout << "  Running DFS from vertex 1 ...\n";
    dfs(gGraph, 0);

    // Ensure a weight matrix exists for Floyd-Warshall
    if (!gWeightReady) {
        std::cout << "\n  No weight matrix found — generating positive-only weights.\n";
        gWeightMatrix = generateWeightMatrix(gGraph, false);
        gWeightReady  = true;
    }

    std::cout << "\n  Running Floyd-Warshall (vertex 1 -> vertex "
              << gGraph.n << ") ...\n";
    floydWarshall(gGraph, gWeightMatrix, 0, gGraph.n - 1);

    std::cout << "\n  (See iteration counts printed above for each algorithm.)\n";
    std::cout << "\n  " << sep << "\n";
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY  -  Lab 2\n"
              << "========================================\n"
              << "  1. Generate graph\n"
              << "  2. Show graph\n"
              << "  -----  Lab 2  -----\n"
              << "  3. DFS traversal\n"
              << "  4. Shortest path : Floyd-Warshall\n"
              << "  5. Algorithm comparison (iteration counts)\n"
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
            case 1: menuGenerateGraph();  break;
            case 2: menuShowGraph();      break;
            case 3: menuDFS();            break;
            case 4: menuFloydWarshall();  break;
            case 5: menuCompare();        break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}