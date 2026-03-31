#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include "graph.h"
#include "tree_generator.h"
#include "eccentricity.h"
#include "shimbell.h"
#include "path_counter.h"

// ── Global state ──────────────────────────────────────────────────────────────
static Graph   gGraph;
static Matrix  gWeightMatrix;
static bool    gGraphReady  = false;
static bool    gWeightReady = false;

// ── Helpers ───────────────────────────────────────────────────────────────────
static inline int D(int v) { return v + 1; }   // display (0-based -> 1-based)

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

static void requireGraph() {
    if (!gGraphReady)
        std::cout << "  No graph generated yet. Use option 1 first.\n";
}

// ── Menu handlers ─────────────────────────────────────────────────────────────

void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }

    std::cout << "  Graph type:\n"
              << "    1. Undirected \n"
              << "    2. Directed    \n";
    int choice = readInt("  Choose [1/2]: ");
    bool directed = (choice == 2);

    gGraph       = generateTree(n, directed);
    gGraphReady  = true;

    // Auto-generate weight matrix (mixed mode) so option 2 can show it immediately
    gWeightMatrix = generateWeightMatrix(gGraph, MIXED);
    gWeightReady  = true;

    // Option 1: brief — just the adjacency matrix
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated.\n"
              << "  (Use option 2 for full details including weight matrix.)\n";
}

void menuShowGraph() {
    if (!gGraphReady) { requireGraph(); return; }
    // Option 2: full view — adj list + edge list + adj matrix (no weight matrix here)
    gGraph.printFull(nullptr);
}

void menuEccentricity() {
    if (!gGraphReady) { requireGraph(); return; }
    auto m = computeMetrics(gGraph);
    printMetrics(m);
}

// Pretty-print the Shimbell weight / result matrix with 1-indexed headers
static void printShimbellMatrix(const Matrix& M) {
    int n = static_cast<int>(M.size());
    const int W = 8;

    std::cout << "       ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(W) << D(j);
    std::cout << "\n  " << std::string(5 + n * W, '-') << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++) {
            if (M[i][j] >= SHIMBELL_INF / 2)
                std::cout << std::setw(W) << "INF";
            else if (M[i][j] <= SHIMBELL_NINF / 2)
                std::cout << std::setw(W) << "-INF";
            else
                std::cout << std::setw(W) << static_cast<long long>(M[i][j]);
        }
        std::cout << "\n";
    }
}

void menuShimbell() {
    if (!gGraphReady) { requireGraph(); return; }

    const std::string sep(60, '-');

    // ── Generate / show weight matrix ─────────────────────────
    // A mixed-mode matrix was auto-created in option 1.
    // Ask whether to keep it or regenerate with a specific mode.
    std::cout << "\n  Current weight matrix was generated with MIXED mode.\n"
              << "  Regenerate with a specific mode? [1=yes / 0=keep]: ";
    if (readInt("") == 1) {
        std::cout << "  Weight mode:\n"
                  << "    1. Positive only\n"
                  << "    2. Negative only\n"
                  << "    3. Mixed (positive and negative)\n";
        int wChoice = readInt("  Choose [1/2/3]: ");
        WeightMode mode;
        switch (wChoice) {
            case 2:  mode = NEGATIVE; break;
            case 3:  mode = MIXED;    break;
            default: mode = POSITIVE; break;
        }
        gWeightMatrix = generateWeightMatrix(gGraph, mode);
    }

    std::cout << "\n  " << sep << "\n";
    std::cout << "  WEIGHT MATRIX  (INF = no edge, 0 = self)\n";
    std::cout << "  " << sep << "\n\n";
    printShimbellMatrix(gWeightMatrix);

    // ── k ─────────────────────────────────────────────────────
    int k = readInt("\n  Enter k (number of steps): ");
    if (k < 0) { std::cout << "  k must be >= 0.\n"; return; }

    auto [minMat, maxMat] = shimbell(gWeightMatrix, k);

    std::cout << "\n  " << sep << "\n";
    std::cout << "  SHIMBELL  MIN-path matrix  (k=" << k << ")\n";
    std::cout << "  " << sep << "\n\n";
    printShimbellMatrix(minMat);

    std::cout << "\n  " << sep << "\n";
    std::cout << "  SHIMBELL  MAX-path matrix  (k=" << k << ")\n";
    std::cout << "  " << sep << "\n\n";
    printShimbellMatrix(maxMat);

}

void menuPaths() {
    if (!gGraphReady) { requireGraph(); return; }

    gGraph.printFull(&gWeightMatrix);   // show the graph so the user can pick vertices easily
    std::cout << "  Vertices are numbered 1 to " << gGraph.n << ".\n";
    int src = readInt("  Source vertex: ")      - 1;   // convert to 0-based
    int dst = readInt("  Destination vertex: ") - 1;

    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex index out of range [1, " << gGraph.n << "].\n";
        return;
    }

    auto paths = findAllPaths(gGraph, src, dst);
    printPaths(paths, src, dst);
}

// ── Main menu ─────────────────────────────────────────────────────────────────

void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY  -  Lab 1\n"
              << "========================================\n"
              << "  1. Generate random connected acyclic graph\n"
              << "  2. Show adjacency list / edge list / matrix\n"
              << "  3. Compute eccentricities, center, diametral vertices\n"
              << "  4. Shimbell's method (min/max paths)\n"
              << "  5. Find routes between two vertices\n"
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
            case 1: menuGenerateGraph(); break;
            case 2: menuShowGraph();     break;
            case 3: menuEccentricity();  break;
            case 4: menuShimbell();      break;
            case 5: menuPaths();         break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
