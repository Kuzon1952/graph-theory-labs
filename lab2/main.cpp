#include <iostream>
#include <limits>
#include <string>
#include "graph.h"
#include "dag_generator.h"
// #include "dfs.h"           // Lab 2 — DFS traversal (in progress)
// #include "floyd_warshall.h" // Lab 2 — Floyd-Warshall shortest path (in progress)

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

    std::cout << "  Graph type:\n"
              << "    1. Directed\n"
              << "    2. Undirected (derived from directed)\n";
    int choice = readInt("  Choose [1/2]: ");
    bool directed = (choice == 1);

    gGraph = generateDAG(n, directed);
    gGraphReady = true;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated.\n";
}

// ── DFS traversal ─────────────────────────────────────────────────────────────
// Visits all vertices reachable from a user-chosen start vertex.
// Outputs the traversal order and iteration count.
// Implementation: dfs.h / dfs.cpp (in progress)
void menuDFS() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    int start = readInt("  Enter start vertex: ") - 1;
    if (start < 0 || start >= gGraph.n) { std::cout << "  Vertex out of range.\n"; return; }
    std::cout << "  DFS from vertex " << start + 1 << " — in progress.\n";
    // dfs(gGraph, start);
}

// ── Floyd-Warshall shortest path ──────────────────────────────────────────────
// Builds the full distance matrix for all pairs of vertices.
// User then picks two vertices to see the shortest path and its sequence.
// Supports positive and negative weights (user chooses at runtime).
// Outputs: distance matrix + path sequence + iteration count.
// Implementation: floyd_warshall.h / floyd_warshall.cpp (in progress)
void menuFloydWarshall() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    std::cout << "  Weight mode:\n"
              << "    1. Positive only\n"
              << "    2. With negative weights\n";
    int mode = readInt("  Choose [1/2]: ");
    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Destination vertex: ") - 1;
    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "  Floyd-Warshall (" << (mode == 1 ? "positive" : "negative")
              << " weights) from " << src+1 << " to " << dst+1
              << " — in progress.\n";
    // floydWarshall(gGraph, src, dst, mode);
}

// ── Algorithm comparison ──────────────────────────────────────────────────────
// Runs DFS and Floyd-Warshall on the same graph and compares their
// iteration counts to show relative performance.
// Implementation: uses dfs.h + floyd_warshall.h (in progress)
void menuCompare() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    std::cout << "  Comparing DFS vs Floyd-Warshall iteration counts — in progress.\n";
    // compareAlgorithms(gGraph);
}

void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY  -  Lab 2\n"
              << "========================================\n"
              << "  1. Generate graph\n"
              << "  2. Show graph\n"
              << "  ── Lab 2 ──\n"
              << "  3. DFS traversal\n"
              << "  4. Shortest path — Floyd-Warshall\n"
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
            case 2:
                if (gGraphReady) gGraph.printFull(nullptr);
                else { std::cout << "  Generate a graph first.\n"; }
                break;
            case 3: menuDFS();            break;
            case 4: menuFloydWarshall();  break;
            case 5: menuCompare();        break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
