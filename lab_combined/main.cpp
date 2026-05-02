#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include <cmath>
#include "graph.h"
#include "dag_generator.h"
#include "weight_matrix.h"
#include "eccentricity.h"
#include "shimbell.h"
#include "path_counter.h"
#include "dfs.h"
#include "floyd_warshall.h"
#include "ford_fulkerson.h"
#include "min_cost_flow.h"
#include "boruvka.h"
#include "edge_cover.h"
#include "kirchhoff.h"
#include "prufer.h"


// ── Global state ──────────────────────────────────────────────────────────────
static Graph  gGraph;
static bool   gGraphReady = false;

// Shared weight matrix (used by Lab 1 Shimbell, Lab 2 Floyd-Warshall, Lab 4 MST)
static Matrix gWeightMatrix;
static bool   gWeightReady = false;

// static std::vector<std::vector<double>> gFWWeightMatrix;  // removed: FW now uses gWeightMatrix directly
// static bool gFWWeightReady = false;

// Lab 3
static std::vector<std::vector<int>> gCapMatrix;
static std::vector<std::vector<int>> gCostMatrix;
static bool gFlowMatricesReady = false;

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
static bool requireLab4Undirected() {
    if (!gGraphReady) {
        requireGraph();
        return false;
    }
    if (gGraph.directed) {
        std::cout << "  Error: Lab 4 requires an undirected graph.\n"
                  << "  Please generate an undirected graph (option 1, choose 2).\n";
        return false;
    }
    return true;
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
    gFlowMatricesReady = false;
    gMSTReady          = false;
    gPruferReady       = false;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated";
    if (!directed) std::cout << " (undirected)";
    std::cout << ".\n  Generate weight matrix with option 3.\n";
}

// ── 3. Generate weight matrix (shared) ───────────────────────────────────────
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
    if (!gGraphReady)  { requireGraph();  return; }
    if (!gWeightReady) { requireWeight(); return; }

    // Build FW matrix from shared weight matrix: Shimbell has INF on diagonal,
    // Floyd-Warshall needs 0 there.
    const int n = gGraph.n;
    std::vector<std::vector<double>> fw(gWeightMatrix);
    for (int i = 0; i < n; i++) fw[i][i] = 0.0;

    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Destination vertex: ") - 1;
    if (src < 0 || src >= n || dst < 0 || dst >= n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    FloydResult res = floydWarshall(gGraph, fw);
    printFloydResult(res, fw, n, src, dst);
}

// ── 9. Generate capacity and cost matrices (Lab 3) ────────────────────────────
void menuGenerateFlowMatrices() {
    if (!gGraphReady) { requireGraph(); return; }
    if (!gGraph.directed) {
        std::cout << "  Error: Lab 3 requires a directed graph.\n"
                  << "  Please generate a directed graph (option 1, choose 1).\n";
        return;
    }
    gCapMatrix  = generateCapacityMatrix(gGraph);
    gCostMatrix = generateCostMatrix(gGraph);
    gFlowMatricesReady = true;
    const int n = gGraph.n;
    printCapacityMatrix(gCapMatrix, n);
    printCostMatrix(gCostMatrix, n);
    // Show out-degree / in-degree to help pick source and sink
    std::cout << "\n  Vertex degrees (to help choose source / sink):\n";
    std::cout << "    Vertex | Out-degree | In-degree\n";
    std::cout << "    " << std::string(34, '-') << "\n";
    for (int i = 0; i < n; i++) {
        int out = 0, in = 0;
        for (int j = 0; j < n; j++) {
            if (gCapMatrix[i][j] > 0) out++;
            if (gCapMatrix[j][i] > 0) in++;
        }
        std::cout << "      " << std::setw(3) << i+1
                  << "   |    " << std::setw(3) << out
                  << "     |   " << std::setw(3) << in << "\n";
    }
    std::cout << "  Good source: high out-degree, low in-degree.\n";
    std::cout << "  Good sink:   high in-degree,  low out-degree.\n";
}

// ── 10. Ford-Fulkerson ────────────────────────────────────────────────────────
void menuFordFulkerson() {
    if (!gGraphReady)        { requireGraph();        return; }
    if (!gFlowMatricesReady) { requireFlowMatrices(); return; }
    const int n = gGraph.n;
    int src = readInt("  Source vertex: ") - 1;
    int dst = readInt("  Sink vertex:   ") - 1;
    if (src < 0 || src >= n || dst < 0 || dst >= n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    if (src == dst) {
        std::cout << "  Source and sink must be different vertices.\n"; return;
    }
    FFResult res = fordFulkerson(n, gCapMatrix, src, dst);
    if (res.maxFlow == 0)
        std::cout << "  No path exists from vertex " << src+1 << " to vertex " << dst+1
                  << ".\n  Max flow = 0. Choose a different source/sink pair.\n"
                  << "  Hint: pick a vertex with only outgoing edges as source,\n"
                  << "        and a vertex with only incoming edges as sink.\n";
    else
        printFFResult(res, gCapMatrix, n);
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
    if (src == dst) {
        std::cout << "  Source and sink must be different vertices.\n"; return;
    }
    MCFResult res = minCostFlow(n, gCapMatrix, gCostMatrix, src, dst);
    printMCFResult(res, gCapMatrix, gCostMatrix, n);
}

// ── 12. Kirchhoff (spanning tree count) ──────────────────────────────────────
void menuKirchhoff() {
    if (!requireLab4Undirected()) { return; }
    long long count = countSpanningTrees(gGraph);
    printKirchhoffResult(gGraph, count);
}

// ── 13. Boruvka (MST) ─────────────────────────────────────────────────────────
void menuBoruvka() {
    if (!requireLab4Undirected()) { return; }
    if (!gWeightReady) { requireWeight(); return; }
    gMST = boruvka(gGraph, gWeightMatrix);
    gMSTReady    = true;
    gPruferReady = false;
    printBoruvkaResult(gMST);
}

// ── 14. Edge cover ────────────────────────────────────────────────────────────
void menuEdgeCover() {
    if (!requireLab4Undirected()) { return; }
    if (!gWeightReady) { requireWeight(); return; }
    std::cout << "  Work on:\n    1. MST (requires Boruvka, option 13)\n    2. Full graph\n";
    bool useMST = (readInt("  Choose [1/2]: ") == 1);
    if (useMST && !gMSTReady) { requireMST(); return; }
    EdgeCoverResult res = minEdgeCover(gGraph, gWeightMatrix, useMST,
                                       useMST ? gMST.edges : std::vector<MSTEdge>{});
    printEdgeCoverResult(res);
}

// ── 15. Prufer encode / decode ────────────────────────────────────────────────
void menuPrufer() {
    if (!requireLab4Undirected()) { return; }
    if (!gMSTReady) { requireMST(); return; }
    if ((int)gMST.edges.size() != gGraph.n - 1) {
        std::cout << "  Error: Prüfer coding requires a spanning tree with n-1 edges.\n"
                  << "  Re-run Boruvka on a connected undirected weighted graph.\n";
        return;
    }
    gPrufer = pruferEncode(gMST.edges, gGraph.n);
    gPruferReady = true;
    printPruferResult(gPrufer, gGraph.n);
    auto decoded = pruferDecode(gPrufer.code, gPrufer.weights, gGraph.n);
    printDecodedTree(decoded);
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
              << "  =====Lab 3=====\n"
              << "  9.  Generate capacity and cost matrices\n"
              << "  10. Max flow: Ford-Fulkerson\n"
              << "  11. Min-cost flow\n"
              << "  =====Lab 4=====\n"
              << "  12. Kirchhoff: count spanning trees\n"
              << "  13. Boruvka: minimum spanning tree\n"
              << "  14. Minimum edge cover\n"
              << "  15. Prufer encode / decode\n"
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
            case 8:  menuFloydWarshall();        break;
            case 9:  menuGenerateFlowMatrices(); break;
            case 10: menuFordFulkerson();        break;
            case 11: menuMinCostFlow();          break;
            case 12: menuKirchhoff();            break;
            case 13: menuBoruvka();              break;
            case 14: menuEdgeCover();            break;
            case 15: menuPrufer();               break;
            case 0:  std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
