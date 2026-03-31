#include "path_counter.h"
#include <iostream>
#include <iomanip>

static inline int D(int v) { return v + 1; }

// ── DFS that collects full paths ──────────────────────────────────────────────
static void dfs(const Graph& g, int u, int dst,
                std::vector<bool>& visited,
                std::vector<int>& current,
                std::vector<std::vector<int>>& result) {
    if (u == dst) {
        result.push_back(current);
        return;
    }
    visited[u] = true;
    for (int v : g.neighbors(u)) {
        if (!visited[v]) {
            current.push_back(v);
            dfs(g, v, dst, visited, current, result);
            current.pop_back();
        }
    }
    visited[u] = false;
}

std::vector<std::vector<int>> findAllPaths(const Graph& g, int src, int dst) {
    std::vector<std::vector<int>> result;
    if (src < 0 || src >= g.n || dst < 0 || dst >= g.n)
        return result;
    if (src == dst) {
        result.push_back({src});
        return result;
    }
    std::vector<bool> visited(g.n, false);
    std::vector<int>  current = {src};
    dfs(g, src, dst, visited, current, result);
    return result;
}

bool pathExists(const Graph& g, int src, int dst) {
    return !findAllPaths(g, src, dst).empty();
}

int countPaths(const Graph& g, int src, int dst) {
    return static_cast<int>(findAllPaths(g, src, dst).size());
}

void printPaths(const std::vector<std::vector<int>>& paths, int src, int dst) {
    std::cout << "\n  Path from " << D(src) << " to " << D(dst) << ": ";
    if (paths.empty()) {
        std::cout << "DOES NOT EXIST\n";
        return;
    }
    std::cout << "EXISTS  (" << paths.size() << " route"
              << (paths.size() == 1 ? "" : "s") << ")\n\n";
    std::cout << "  Routes:\n";
    for (int i = 0; i < (int)paths.size(); i++) {
        std::cout << "    Route " << i + 1 << ":  { ";
        for (int j = 0; j < (int)paths[i].size(); j++) {
            if (j) std::cout << " -> ";
            std::cout << D(paths[i][j]);
        }
        std::cout << " }\n";
    }
}
