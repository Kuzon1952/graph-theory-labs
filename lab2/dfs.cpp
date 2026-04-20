#include "dfs.h"
#include <iostream>
#include <stack>
#include <vector>
#include <iomanip>

// Iterative DFS using an explicit stack.
// Counts every "iteration" — each time we pop a vertex and inspect its neighbors.
void dfs(const Graph& g, int start) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  DFS TRAVERSAL  (start = vertex " << start + 1 << ")\n";
    std::cout << "  " << sep << "\n\n";

    std::vector<bool> visited(g.n, false);
    std::vector<int>  order;
    std::stack<int>   stk;
    long long         iterations = 0;

    stk.push(start);

    while (!stk.empty()) {
        int u = stk.top();
        stk.pop();
        iterations++;

        if (visited[u]) continue;
        visited[u] = true;
        order.push_back(u);
        iterations++;

        // Push neighbors in reverse order so smaller-index neighbors are
        // visited first (matches recursive DFS order).
        auto nb = g.neighbors(u);
        for (int i = (int)nb.size() - 1; i >= 0; i--) {
            iterations++;
            if (!visited[nb[i]])
                stk.push(nb[i]);
        }
    }

    // ── Traversal order ───────────────────────────────────────
    std::cout << "  Traversal order:\n  ";
    for (int i = 0; i < (int)order.size(); i++) {
        if (i) std::cout << " -> ";
        std::cout << order[i] + 1;
    }
    std::cout << "\n\n";

    // ── Visited / unreachable ─────────────────────────────────
    std::vector<int> unvisited;
    for (int v = 0; v < g.n; v++)
        if (!visited[v]) unvisited.push_back(v);

    std::cout << "  Vertices visited   : " << order.size() << " / " << g.n << "\n";
    if (!unvisited.empty()) {
        std::cout << "  Unreachable        : ";
        for (int i = 0; i < (int)unvisited.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << unvisited[i] + 1;
        }
        std::cout << "\n";
        std::cout << "  (Graph not fully reachable from vertex " << start + 1 << ")\n";
    }

    std::cout << "\n  Total iterations   : " << iterations << "\n";
    std::cout << "\n  " << sep << "\n";
}
