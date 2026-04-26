#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
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
#include "kirchhoff.h"
#include "boruvka.h"
#include "prufer.h"
#include "edge_cover.h"

// ── Global state ──────────────────────────────────────────────────────────────
static Graph  gGraph;
static bool   gGraphReady = false;

// Lab 1 / Lab 2 weight matrices
static Matrix gWeightMatrix;        // Shimbell weight matrix
static bool   gWeightReady = false;

static std::vector<std::vector<double>> gFWWeightMatrix;
static bool gFWWeightReady = false;

// Lab 3
static std::vector<std::vector<int>> gCapacity;
static std::vector<std::vector<int>> gCostMat;
static bool gFlowMatricesReady = false;
static FFResult  gFFResult;
static bool      gFFDone = false;

// Lab 4
static BoruvkaResult gMST;
static bool          gMSTReady   = false;
static PruferResult  gPrufer;
static bool          gPruferReady = false;

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
    std::cout << "  Error: No graph generated yet. Use option 1 first.\n";
}
static void requireWeight() {
    std::cout << "  Error: No weight matrix generated. Use option 3 first.\n";
}
static void requireFlowMatrices() {
    std::cout << "  Error: No capacity/cost matrices. Use option 9 first.\n";
}
static void requireMST() {
    std::cout << "  Error: No MST computed. Use option 13 (Boruvka) first.\n";
}

// ── 1. Generate graph ─────────────────────────────────────────────────────────
void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }
    std::cout << "  Graph type:\n    1. Directed\n    2. Undirected\n";
    bool directed = (readInt("  Choose [1/2]: ") == 1);
    gGraph = generateDAG(n, directed);
    gGraphReady        = true;
    gWeightReady       = false;
    gFWWeightReady     = false;
    gFlowMatricesReady = false;
    gFFDone            = false;
    gMSTReady          = false;
    gPruferReady       = false;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated";
    if (!directed) std::cout << " (undirected)";
    std::cout << ".\n  Generate weight matrix with option 3.\n";
}

// ── 3. Generate weight matrix (Shimbell) ─────────────────────────────────────
void menuGenerateWeightMatrix() {
    if (!gGraphReady) { requireGraph(); return; }
    std::cout << "  Weight mode:\n    1. Positive only\n    2. Negative only\n    3. Mixed\n";
    int wChoice = readInt("  Choose [1/2/3]: ");
    WeightMode mode;
    switch (wChoice) {
        case 2:  mode = NEGATIVE; break;
        case 3:  mode = MIXED;    break;
        default: mode = POSITIVE; break;
    }
    gWeightMatrix = generateWeightMatrix(gGraph, mode);
    gWeightReady  = true;
    gMSTReady     = false;
    gPruferReady  = false;
    std::cout << "  Weight matrix generated.\n";
    // print it
    const int W = 8, n = gGraph.n;
    std::cout << "\n       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(W) << D(j);
    std::cout << "\n  " << std::string(5 + n * W, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++) {
            double v = gWeightMatrix[i][j];
            if      (v >= SHIMBELL_INF / 2)  std::cout << std::setw(W) << "INF";
            else if (v <= SHIMBELL_NINF / 2) std::cout << std::setw(W) << "-INF";
            else    std::cout << std::setw(W) << static_cast<long long>(v);
        }
        std::cout << "\n";
    }
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

// ── 4. Eccentricities ─────────────────────────────────────────────────────────
void menuEccentricity() {
    if (!gGraphReady) { requireGraph(); return; }
    auto m = computeMetrics(gGraph);
    printMetrics(m);
}

// ── 5. Shimbell ───────────────────────────────────────────────────────────────
void menuShimbell() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gWeightReady) { requireWeight(); return; }
    const std::string sep(60, '-');
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

// ── 6. Routes ─────────────────────────────────────────────────────────────────
void menuPaths() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gWeightReady) { requireWeight(); return; }
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

// ── 7. DFS ───────────────────────────────────────────────────────────────────
void menuDFS() {
    if (!gGraphReady) { requireGraph(); return; }
    int start = readInt("  Enter start vertex: ") - 1;
    if (start < 0 || start >= gGraph.n) { std::cout << "  Vertex out of range.\n"; return; }
    dfs(gGraph, start);
}

// ── 8. Floyd-Warshall ─────────────────────────────────────────────────────────
void menuFloydWarshall() {
    if (!gGraphReady) { requireGraph(); return; }

    if (gFWWeightReady) {
        std::cout << "  A Floyd-Warshall weight matrix already exists.\n"
                  << "    1. Reuse it\n    2. Generate a new one\n";
        if (readInt("  Choose [1/2]: ") != 1) gFWWeightReady = false;
    }
    if (!gFWWeightReady) {
        std::cout << "  Weight mode:\n"
                  << "    1. Positive only\n"
                  << "    2. Negative only\n"
                  << "    3. Mixed\n";
        int wc = readInt("  Choose [1/2/3]: ");
        int mode = (wc == 2) ? 1 : (wc == 3) ? 2 : 0;
        gFWWeightMatrix = generateWeightMatrix(gGraph, mode);
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

// ── 9. Generate capacity and cost matrices ────────────────────────────────────
void menuGenerateFlowMatrices() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gGraph.directed) {
        std::cout << "  Error: Lab 3 requires a directed graph.\n"
                  << "  Please generate a directed graph (option 1, choose 1).\n";
        return;
    }
    gCapacity          = generateCapacityMatrix(gGraph);
    gCostMat           = generateCostMatrix(gGraph);
    gFlowMatricesReady = true;
    gFFDone            = false;
    std::cout << "  Capacity and cost matrices generated.\n";
    printCapacityMatrix(gCapacity, gGraph.n);
    printCostMatrix(gCostMat, gGraph.n);
}

// ── 10. Max flow: Ford-Fulkerson ──────────────────────────────────────────────
void menuMaxFlow() {
    if (!gGraphReady)        { requireGraph();        return; }
    if (!gFlowMatricesReady) { requireFlowMatrices(); return; }
    const int n = gGraph.n;
    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Sink vertex:   ") - 1;
    if (src < 0 || src >= n || dst < 0 || dst >= n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "\n  Augmenting paths:\n";
    gFFResult = fordFulkerson(n, gCapacity, src, dst);
    gFFDone   = true;
    printFFResult(gFFResult, gCapacity, n);
}

// ── 11. Min-cost flow ─────────────────────────────────────────────────────────
void menuMinCostFlow() {
    if (!gGraphReady)        { requireGraph();        return; }
    if (!gFlowMatricesReady) { requireFlowMatrices(); return; }
    const int n = gGraph.n;
    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Sink vertex:   ") - 1;
    if (src < 0 || src >= n || dst < 0 || dst >= n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "\n  Computing max flow (silent)...\n";
    MCFResult res = minCostFlow(n, gCapacity, gCostMat, src, dst);
    printMCFResult(res, gCapacity, gCostMat, n);
}

// ── 12. Kirchhoff: count spanning trees ───────────────────────────────────────
void menuKirchhoff() {
    if (!gGraphReady) { requireGraph(); return; }
    long long count = countSpanningTrees(gGraph);
    printKirchhoffResult(gGraph, count);
}

// ── 13. Borůvka: minimum spanning tree ───────────────────────────────────────
void menuBoruvka() {
    if (!gGraphReady)  { requireGraph();  return; }
    if (!gWeightReady) { requireWeight(); return; }
    std::cout << "\n  Running Boruvka's algorithm:\n";
    gMST      = boruvka(gGraph, gWeightMatrix);
    gMSTReady = true;
    gPruferReady = false;
    printBoruvkaResult(gMST);
}

// ── 14. Prüfer encode ─────────────────────────────────────────────────────────
void menuPruferEncode() {
    if (!gMSTReady) { requireMST(); return; }
    const int n = gGraph.n;
    if (n < 2) { std::cout << "  Need at least 2 vertices.\n"; return; }
    std::cout << "\n  Prufer encoding steps:\n";
    gPrufer      = pruferEncode(gMST.edges, n);
    gPruferReady = true;
    printPruferResult(gPrufer, n);
}

// ── 15. Prüfer decode ─────────────────────────────────────────────────────────
void menuPruferDecode() {
    if (!gPruferReady) {
        std::cout << "  Error: No Prufer code stored. Use option 14 first.\n";
        return;
    }
    const int n = gGraph.n;
    std::cout << "\n  Prufer decoding steps:\n";
    auto edges = pruferDecode(gPrufer.code, gPrufer.weights, n);
    printDecodedTree(edges);
}

// ── 16. Minimum edge cover ────────────────────────────────────────────────────
void menuEdgeCover() {
    if (!gGraphReady)  { requireGraph();  return; }
    if (!gWeightReady) { requireWeight(); return; }

    std::cout << "  Apply to:\n"
              << "    1. Original graph (undirected view)\n"
              << "    2. MST (Boruvka result)\n";
    int choice = readInt("  Choose [1/2]: ");
    bool useMST = (choice == 2);

    if (useMST && !gMSTReady) { requireMST(); return; }

    EdgeCoverResult res = minEdgeCover(gGraph, gWeightMatrix,
                                       useMST, gMST.edges);
    printEdgeCoverResult(res);
}

// ── Menu ──────────────────────────────────────────────────────────────────────
void printMenu() {
    std::cout << "\n========================================\n"
              << "   GRAPH THEORY LABS\n"
              << "========================================\n"
              << "  1. Generate graph\n"
              << "  2. Show graph (adjacency list / matrix)\n"
              << "  3. Generate weight matrix\n"
              << "  =====Lab 1=====\n"
              << "  4. Eccentricities, center, diametral vertices\n"
              << "  5. Shimbell's method (min/max paths, k steps)\n"
              << "  6. Find all routes between two vertices\n"
              << "  =====Lab 2=====\n"
              << "  7. DFS traversal\n"
              << "  8. Shortest path: Floyd-Warshall\n"
              << "  =====Lab 3: Flows=====\n"
              << "  9.  Generate capacity and cost matrices\n"
              << "  10. Maximum flow (Ford-Fulkerson)\n"
              << "  11. Minimum-cost flow  [theta = floor(2/3 * maxFlow)]\n"
              << "  =====Lab 4: Trees=====\n"
              << "  12. Count spanning trees (Kirchhoff theorem)\n"
              << "  13. Minimum spanning tree (Boruvka)\n"
              << "  14. Encode MST as Prufer code with weights\n"
              << "  15. Decode Prufer code, restore weights\n"
              << "  16. Minimum edge cover\n"
              << "  0.  Exit\n"
              << "----------------------------------------\n";
}

int main() {
    int choice = -1;
    while (choice != 0) {
        printMenu();
        choice = readInt("  Enter choice: ");
        std::cout << "\n";
        switch (choice) {
            case 1:  menuGenerateGraph();    break;
            case 2:
                if (gGraphReady)
                    gGraph.printFull(gWeightReady ? &gWeightMatrix : nullptr);
                else requireGraph();
                break;
            case 3:  menuGenerateWeightMatrix(); break;
            case 4:  menuEccentricity();         break;
            case 5:  menuShimbell();             break;
            case 6:  menuPaths();                break;
            case 7:  menuDFS();                  break;
            case 8:  menuFloydWarshall();         break;
            case 9:  menuGenerateFlowMatrices(); break;
            case 10: menuMaxFlow();              break;
            case 11: menuMinCostFlow();          break;
            case 12: menuKirchhoff();            break;
            case 13: menuBoruvka();              break;
            case 14: menuPruferEncode();         break;
            case 15: menuPruferDecode();         break;
            case 16: menuEdgeCover();            break;
            case 0:  std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
