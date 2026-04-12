#include <iostream>
#include <limits>
#include <string>
#include <iomanip>
#include "graph.h"
#include "dag_generator.h"
#include "eccentricity.h"
#include "shimbell.h"
#include "path_counter.h"
// #include "dfs.h"            // Lab 2 — DFS traversal (in progress)
// #include "floyd_warshall.h" // Lab 2 — Floyd-Warshall shortest path (in progress)
// #include "ford_fulkerson.h" // Lab 3 — max flow (in progress)
// #include "min_cost_flow.h"  // Lab 3 — min cost flow at 2/3*max (in progress)

// ── Global state ──────────────────────────────────────────────────────────────
static Graph  gGraph;
static Matrix gWeightMatrix;
static bool   gGraphReady  = false;
static bool   gWeightReady = false;

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
    if (!gGraphReady)
        std::cout << "  No graph generated yet. Use option 1 first.\n";
}

// ── Generate graph ────────────────────────────────────────────────────────────
void menuGenerateGraph() {
    int n = readInt("  Number of vertices: ");
    if (n <= 0) { std::cout << "  Must be > 0.\n"; return; }

    std::cout << "  Graph type:\n"
              << "    1. Directed\n"
              << "    2. Undirected (derived from directed)\n";
    int choice = readInt("  Choose [1/2]: ");
    bool directed = (choice == 1);

    gGraph = generateDAG(n, directed);
    gGraphReady   = true;
    gWeightMatrix = generateWeightMatrix(gGraph, MIXED);
    gWeightReady  = true;

    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated";
    if (!directed) std::cout << " (undirected, derived from directed DAG)";
    std::cout << ".\n  (Use option 2 for full details.)\n";
}

// ── Shimbell pretty-print helper ──────────────────────────────────────────────
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
        std::cout << "  Weight mode:\n"
                  << "    1. Positive only\n"
                  << "    2. Negative only\n"
                  << "    3. Mixed\n";
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

// ── Lab 2: DFS traversal ─────────────────────────────────────────────────────
// Visits all vertices reachable from a user-chosen start vertex.
// Outputs the traversal order and iteration count.
// Implementation: dfs.h / dfs.cpp (in progress)
void menuDFS() {
    if (!gGraphReady) { requireGraph(); return; }
    int start = readInt("  Enter start vertex: ") - 1;
    if (start < 0 || start >= gGraph.n) { std::cout << "  Vertex out of range.\n"; return; }
    std::cout << "  DFS from vertex " << start + 1 << " — in progress.\n";
    // dfs(gGraph, start);
}

// ── Lab 2: Floyd-Warshall shortest path ──────────────────────────────────────
// Builds the full distance matrix for all pairs of vertices.
// User picks two vertices to see the shortest path and its sequence.
// Supports positive and negative weights (user chooses at runtime).
// Outputs: distance matrix + path sequence + iteration count.
// Implementation: floyd_warshall.h / floyd_warshall.cpp (in progress)
void menuFloydWarshall() {
    if (!gGraphReady) { requireGraph(); return; }
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

// ── Lab 2: Algorithm comparison ───────────────────────────────────────────────
// Runs DFS and Floyd-Warshall on the same graph and compares their
// iteration counts to show relative performance.
// Implementation: uses dfs.h + floyd_warshall.h (in progress)
void menuCompare() {
    if (!gGraphReady) { requireGraph(); return; }
    std::cout << "  Comparing DFS vs Floyd-Warshall iteration counts — in progress.\n";
    // compareAlgorithms(gGraph);
}

// ── Lab 3: Generate capacity & cost matrices ──────────────────────────────────
// Randomly generates two matrices from the graph's edges:
//   - Capacity matrix: how much flow each edge can carry (positive values)
//   - Cost matrix: cost per unit of flow on each edge
// Both generated using the Normal distribution (same as weight matrix in Lab 1).
// Implementation: ford_fulkerson.h (in progress)
void menuGenerateFlowMatrices() {
    if (!gGraphReady) { requireGraph(); return; }
    std::cout << "  Generating capacity and cost matrices — in progress.\n";
    // generateFlowMatrices(gGraph);
}

// ── Lab 3: Maximum flow — Ford-Fulkerson ─────────────────────────────────────
// Finds the maximum flow from a user-chosen source to a user-chosen sink.
// Uses the Ford-Fulkerson algorithm with BFS (Edmonds-Karp variant).
// Outputs: max flow value + final flow on each edge.
// Implementation: ford_fulkerson.h / ford_fulkerson.cpp (in progress)
void menuMaxFlow() {
    if (!gGraphReady) { requireGraph(); return; }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "  Ford-Fulkerson max flow from " << src+1
              << " to " << snk+1 << " — in progress.\n";
    // fordFulkerson(gGraph, src, snk);
}

// ── Lab 3: Minimum cost flow ──────────────────────────────────────────────────
// Finds the minimum cost flow for a flow value of floor(2/3 * maxFlow).
// Uses Floyd-Warshall (from Lab 2) to find minimum cost augmenting paths.
// Outputs: total cost + flow assignment per edge.
// Implementation: min_cost_flow.h / min_cost_flow.cpp (in progress)
void menuMinCostFlow() {
    if (!gGraphReady) { requireGraph(); return; }
    int src = readInt("  Source vertex: ") - 1;
    int snk = readInt("  Sink vertex: ")   - 1;
    if (src < 0 || src >= gGraph.n || snk < 0 || snk >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    std::cout << "  Min cost flow (2/3 * max) from " << src+1
              << " to " << snk+1 << " — in progress.\n";
    // minCostFlow(gGraph, src, snk);
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
              << "  7. Shortest path — Floyd-Warshall\n"
              << "  8. Algorithm comparison (iteration counts)\n"
              << "  =====Lab 3=====\n"
              << "  9.  Generate capacity & cost matrices\n"
              << "  10. Maximum flow (Ford-Fulkerson)\n"
              << "  11. Minimum cost flow (2/3 * max)\n"
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
                else { requireGraph(); }
                break;
            case 3:  menuEccentricity();         break;
            case 4:  menuShimbell();             break;
            case 5:  menuPaths();                break;
            case 6:  menuDFS();                  break;
            case 7:  menuFloydWarshall();        break;
            case 8:  menuCompare();              break;
            case 9:  menuGenerateFlowMatrices(); break;
            case 10: menuMaxFlow();              break;
            case 11: menuMinCostFlow();          break;
            case 0:  std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
