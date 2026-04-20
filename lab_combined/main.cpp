#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <stack>
#include <cmath>
#include "graph.h"
#include "dag_generator.h"
#include "eccentricity.h"
#include "shimbell.h"
#include "path_counter.h"
#include "dfs.h"
#include "floyd_warshall.h"
#include "ford_fulkerson.h"
#include "min_cost_flow.h"

// ── Global state ──────────────────────────────────────────────────────────────
static Graph  gGraph;
static Matrix gWeightMatrix;        // shimbell weight matrix
static bool   gGraphReady  = false;
static bool   gWeightReady = false;

// FW weight matrix
static std::vector<std::vector<double>> gFWWeightMatrix;
static bool gFWWeightReady = false;

// Lab 3 matrices
static FMatrix gCapacity;           // Ω — capacity matrix
static FMatrix gCostMatrix;         // D — cost matrix
static bool    gFlowMatricesReady = false;
static FFResult gFFResult;          // last Ford-Fulkerson result
static bool    gFFDone = false;

static inline int D(int v) { return v + 1; }

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
    std::cout << "  No graph generated yet. Use option 1 first.\n";
}

// ── Generate graph ────────────────────────────────────────────────────────────
void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }
    std::cout << "  Graph type:\n    1. Directed\n    2. Undirected (derived from directed)\n";
    bool directed = (readInt("  Choose [1/2]: ") == 1);
    gGraph = generateDAG(n, directed);
    gGraphReady   = true;
    gWeightMatrix = generateWeightMatrix(gGraph, MIXED);
    gWeightReady  = true;
    gFWWeightReady = false;
    gFlowMatricesReady = false;
    gFFDone = false;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated";
    if (!directed) std::cout << " (undirected, derived from directed DAG)";
    std::cout << ".\n  (Use option 2 for full details.)\n";
}

// ── Shimbell pretty-print ─────────────────────────────────────────────────────
static void printShimbellMatrix(const Matrix& M) {
    int n = static_cast<int>(M.size());
    const int W = 8;
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(W) << D(j);
    std::cout << "\n  " << std::string(5 + n * W, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++) {
            if      (M[i][j] >= SHIMBELL_INF / 2)  std::cout << std::setw(W) << "INF";
            else if (M[i][j] <= SHIMBELL_NINF / 2) std::cout << std::setw(W) << "-INF";
            else    std::cout << std::setw(W) << static_cast<long long>(M[i][j]);
        }
        std::cout << "\n";
    }
}

// ── Lab 1: Eccentricities ─────────────────────────────────────────────────────
void menuEccentricity() {
    if (!gGraphReady) { requireGraph(); return; }
    auto m = computeMetrics(gGraph);
    printMetrics(m);
}

// ── Lab 1: Shimbell ───────────────────────────────────────────────────────────
void menuShimbell() {
    if (!gGraphReady) { requireGraph(); return; }
    const std::string sep(60, '-');
    std::cout << "\n  Current weight matrix mode: MIXED.\n"
              << "  Regenerate with specific mode? [1=yes / 0=keep]: ";
    if (readInt("") == 1) {
        std::cout << "  Weight mode:\n    1. Positive only\n    2. Negative only\n    3. Mixed\n";
        int wChoice = readInt("  Choose [1/2/3]: ");
        WeightMode mode;
        switch (wChoice) {
            case 2:  mode = NEGATIVE; break;
            case 3:  mode = MIXED;    break;
            default: mode = POSITIVE; break;
        }
        gWeightMatrix = generateWeightMatrix(gGraph, mode);
    }
    std::cout << "\n  " << sep << "\n  WEIGHT MATRIX\n  " << sep << "\n\n";
    printShimbellMatrix(gWeightMatrix);
    int k = readInt("\n  Enter k (number of steps): ");
    if (k < 0) { std::cout << "  k must be >= 0.\n"; return; }
    auto [minMat, maxMat] = shimbell(gWeightMatrix, k);
    std::cout << "\n  " << sep << "\n  SHIMBELL MIN-path (k=" << k << ")\n  " << sep << "\n\n";
    printShimbellMatrix(minMat);
    std::cout << "\n  " << sep << "\n  SHIMBELL MAX-path (k=" << k << ")\n  " << sep << "\n\n";
    printShimbellMatrix(maxMat);
}

// ── Lab 1: Routes ─────────────────────────────────────────────────────────────
void menuPaths() {
    if (!gGraphReady) { requireGraph(); return; }
    gGraph.printFull(&gWeightMatrix);
    std::cout << "  Vertices are numbered 1 to " << gGraph.n << ".\n";
    int src = readInt("  Source vertex: ")      - 1;
    int dst = readInt("  Destination vertex: ") - 1;
    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    auto paths = findAllPaths(gGraph, src, dst);
    printPaths(paths, src, dst);
}

// ── Lab 2: DFS ───────────────────────────────────────────────────────────────
void menuDFS() {
    if (!gGraphReady) { requireGraph(); return; }
    int start = readInt("  Enter start vertex: ") - 1;
    if (start < 0 || start >= gGraph.n) { std::cout << "  Vertex out of range.\n"; return; }
    dfs(gGraph, start);
}

// ── Lab 2: Floyd-Warshall ─────────────────────────────────────────────────────
void menuFloydWarshall() {
    if (!gGraphReady) { requireGraph(); return; }

    if (gFWWeightReady) {
        std::cout << "  A Floyd-Warshall weight matrix already exists.\n"
                  << "    1. Reuse it\n    2. Generate a new one\n";
        if (readInt("  Choose [1/2]: ") != 1) gFWWeightReady = false;
    }
    if (!gFWWeightReady) {
        std::cout << "  Weight mode:\n"
                  << "    1. Positive only  [1 .. 20]\n"
                  << "    2. With negative weights  [-10 .. 20]\n";
        bool allowNeg = (readInt("  Choose [1/2]: ") == 2);
        gFWWeightMatrix = generateWeightMatrix(gGraph, allowNeg);
        gFWWeightReady  = true;
    }

    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Destination vertex: ") - 1;
    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    FloydResult res = floydWarshall(gGraph, gFWWeightMatrix);
    printFloydResult(res, gFWWeightMatrix, gGraph.n, src, dst);
}

// ── Lab 3: Generate capacity & cost matrices ──────────────────────────────────
// Randomly generates Ω (capacity) and D (cost) matrices from the graph edges.
void menuGenerateFlowMatrices() {
    if (!gGraphReady) { requireGraph(); return; }
    gCapacity  = generateCapacityMatrix(gGraph);
    gCostMatrix = generateCostMatrix(gGraph);
    gFlowMatricesReady = true;
    gFFDone = false;

    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  FLOW MATRICES GENERATED\n";
    std::cout << "  " << sep << "\n";
    printFMatrix(gCapacity,   gGraph.n, "CAPACITY MATRIX Ω  (0 = no arc):");
    printFMatrix(gCostMatrix, gGraph.n, "COST MATRIX D  (INF = no arc):");
    std::cout << "\n  " << sep << "\n";
}

// ── Lab 3: Maximum flow — Ford-Fulkerson ─────────────────────────────────────
void menuMaxFlow() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gFlowMatricesReady) {
        std::cout << "  No capacity/cost matrices. Use option 9 first.\n"; return;
    }

    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    if (src == snk) { std::cout << "  Source and sink must be different.\n"; return; }

    gFFResult = fordFulkerson(gCapacity, gGraph.n, src, snk);
    gFFDone = true;

    long long theta = static_cast<long long>(std::floor(2.0/3.0 * gFFResult.maxFlow));
    std::cout << "\n  φ_max = " << static_cast<long long>(gFFResult.maxFlow) << "\n";
    std::cout << "  θ = ⌊2/3 × φ_max⌋ = ⌊2/3 × "
              << static_cast<long long>(gFFResult.maxFlow) << "⌋ = " << theta << "\n";
    std::cout << "  (Use option 11 to find minimum cost flow for θ=" << theta << ")\n";
}

// ── Lab 3: Minimum cost flow ──────────────────────────────────────────────────
void menuMinCostFlow() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gFlowMatricesReady) {
        std::cout << "  No capacity/cost matrices. Use option 9 first.\n"; return;
    }
    if (!gFFDone) {
        std::cout << "  Run maximum flow (option 10) first to get φ_max.\n"; return;
    }

    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    if (src == snk) { std::cout << "  Source and sink must be different.\n"; return; }

    double theta = std::floor(2.0/3.0 * gFFResult.maxFlow);
    std::cout << "\n  φ_max = " << static_cast<long long>(gFFResult.maxFlow)
              << "   →   θ = ⌊2/3 × φ_max⌋ = " << static_cast<long long>(theta) << "\n";

    if (theta < 1.0) {
        std::cout << "  θ = 0 — no flow needed.\n"; return;
    }

    minCostFlow(gCapacity, gCostMatrix, gGraph.n, src, snk, theta);
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY LABS\n"
              << "========================================\n"
              << "  1. Generate graph\n"
              << "  2. Show graph (adjacency list / matrix)\n"
              << "  =====Lab 1=====\n"
              << "  3. Eccentricities, center, diametral vertices\n"
              << "  4. Shimbell's method (min/max paths, k steps)\n"
              << "  5. Find all routes between two vertices\n"
              << "  =====Lab 2=====\n"
              << "  6. DFS traversal\n"
              << "  7. Shortest path: Floyd-Warshall\n"
              << "  =====Lab 3=====\n"
              << "  9.  Generate capacity & cost matrices\n"
              << "  10. Maximum flow (Ford-Fulkerson)\n"
              << "  11. Minimum cost flow (θ = ⌊2/3 × max⌋)\n"
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
            case 1:  menuGenerateGraph();        break;
            case 2:
                if (gGraphReady) gGraph.printFull(&gWeightMatrix);
                else requireGraph();
                break;
            case 3:  menuEccentricity();         break;
            case 4:  menuShimbell();             break;
            case 5:  menuPaths();                break;
            case 6:  menuDFS();                  break;
            case 7:  menuFloydWarshall();        break;
            case 9:  menuGenerateFlowMatrices(); break;
            case 10: menuMaxFlow();              break;
            case 11: menuMinCostFlow();          break;
            case 0:  std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
