#include <iostream>
#include <limits>
#include <string>
#include "graph.h"
#include "dag_generator.h"
#include "dfs.h"
#include "floyd_warshall.h"
#include <stack>

static Graph                            gGraph;
static bool                             gGraphReady  = false;
static std::vector<std::vector<double>> gWeightMatrix;
static bool                             gWeightReady = false;

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
    std::cout << "  Graph type:\n    1. Directed\n    2. Undirected\n";
    bool directed = (readInt("  Choose [1/2]: ") == 1);
    gGraph      = generateDAG(n, directed);
    gGraphReady = true;
    gWeightReady = false;
    gGraph.printAdjMatrix();
    std::cout << "  Graph with " << n << " vertices generated ("
              << (directed ? "directed" : "undirected") << ").\n";
}

void menuShowGraph() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    gGraph.printFull(gWeightReady ? &gWeightMatrix : nullptr);
}

void menuDFS() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    int start = readInt("  Enter start vertex (1-based): ") - 1;
    if (start < 0 || start >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }
    dfs(gGraph, start);
}

// Helper: ask for weight matrix, generate if needed
static void ensureWeightMatrix(bool forceNew = false) {
    if (!forceNew && gWeightReady) {
        std::cout << "  A weight matrix already exists.\n"
                  << "    1. Reuse it\n    2. Generate a new one\n";
        if (readInt("  Choose [1/2]: ") == 1) return;
    }
    std::cout << "  Weight mode:\n"
              << "    1. Positive only  [1 .. 20]\n"
              << "    2. With negative weights  [-10 .. 20]\n";
    bool allowNeg = (readInt("  Choose [1/2]: ") == 2);
    gWeightMatrix = generateWeightMatrix(gGraph, allowNeg);
    gWeightReady  = true;
    std::cout << "  Weight matrix generated"
              << (allowNeg ? " (with negative weights)" : " (positive only)") << ".\n";
}

void menuFloydWarshall() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }
    ensureWeightMatrix();

    int src = readInt("  Source vertex (1-based): ") - 1;
    int dst = readInt("  Destination vertex (1-based): ") - 1;
    if (src < 0 || src >= gGraph.n || dst < 0 || dst >= gGraph.n) {
        std::cout << "  Vertex out of range.\n"; return;
    }

    FloydResult res = floydWarshall(gGraph, gWeightMatrix);
    printFloydResult(res, gWeightMatrix, gGraph.n, src, dst);
}

void menuCompare() {
    if (!gGraphReady) { std::cout << "  Generate a graph first (option 1).\n"; return; }

    const std::string sep(60, '-');
    const int n = gGraph.n;

    std::cout << "\n  " << sep << "\n";
    std::cout << "  ALGORITHM COMPARISON\n";
    std::cout << "  " << sep << "\n\n";

    // ── DFS ───────────────────────────────────────────────────
    std::cout << "  Running DFS from vertex 1...\n";
    dfs(gGraph, 0);

    // ── Floyd-Warshall ────────────────────────────────────────
    if (!gWeightReady) {
        std::cout << "\n  No weight matrix — generating positive-only weights.\n";
        gWeightMatrix = generateWeightMatrix(gGraph, false);
        gWeightReady  = true;
    }
    std::cout << "\n  Running Floyd-Warshall (vertex 1 -> vertex " << n << ")...\n";
    FloydResult res = floydWarshall(gGraph, gWeightMatrix);
    printFloydResult(res, gWeightMatrix, n, 0, n - 1);

    // ── Side-by-side summary ──────────────────────────────────
    long long dfsIter = 0;
    {
        // Quick re-run of DFS just to get its iteration count cleanly
        std::vector<bool> vis(n, false);
        std::stack<int>   stk;
        stk.push(0);
        while (!stk.empty()) {
            int u = stk.top(); stk.pop();
            if (vis[u]) continue;
            vis[u] = true;
            for (int v = n - 1; v >= 0; v--) {
                dfsIter++;
                if (gGraph.adj[u][v] && !vis[v]) stk.push(v);
            }
        }
    }

    std::cout << "\n  " << sep << "\n";
    std::cout << "  SUMMARY  (n = " << n << ")\n";
    std::cout << "  " << sep << "\n";
    std::cout << "  DFS            iterations: " << dfsIter
              << "  (O(n²) ~ " << (long long)n*n << ")\n";
    std::cout << "  Floyd-Warshall iterations: " << res.iterations
              << "  (O(n³) ~ " << (long long)n*n*n << ")\n";
    std::cout << "  " << sep << "\n";
}

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
            case 1: menuGenerateGraph(); break;
            case 2: menuShowGraph();     break;
            case 3: menuDFS();           break;
            case 4: menuFloydWarshall(); break;
            case 5: menuCompare();       break;
            case 0: std::cout << "  Goodbye.\n"; break;
            default: std::cout << "  Unknown option.\n"; break;
        }
    }
    return 0;
}
