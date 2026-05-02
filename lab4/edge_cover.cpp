#include "edge_cover.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <algorithm>

// Edmonds blossom maximum matching for a general undirected graph.
// Minimum edge cover size is n - |maximum matching| for graphs without isolated
// vertices, so this keeps the full-graph case correct even when it is not a tree.
class GeneralMatching {
public:
    GeneralMatching(const std::vector<std::vector<int>>& adj, int n)
        : adj(adj), n(n), match(n, -1), parent(n), base(n),
          used(n), blossom(n) {}

    int run(std::vector<int>& outMatch) {
        int result = 0;
        for (int root = 0; root < n; root++) {
            if (match[root] != -1) continue;
            int endpoint = findAugmentingPath(root);
            if (endpoint == -1) continue;

            while (endpoint != -1) {
                int pv = parent[endpoint];
                int next = match[pv];
                match[endpoint] = pv;
                match[pv] = endpoint;
                endpoint = next;
            }
            result++;
        }
        outMatch = match;
        return result;
    }

private:
    const std::vector<std::vector<int>>& adj;
    int n;
    std::vector<int> match;
    std::vector<int> parent;
    std::vector<int> base;
    std::vector<bool> used;
    std::vector<bool> blossom;
    std::queue<int> q;

    int lca(int a, int b) {
        std::vector<bool> seen(n, false);
        while (true) {
            a = base[a];
            seen[a] = true;
            if (match[a] == -1) break;
            a = parent[match[a]];
        }
        while (true) {
            b = base[b];
            if (seen[b]) return b;
            b = parent[match[b]];
        }
    }

    void markPath(int v, int b, int child) {
        while (base[v] != b) {
            blossom[base[v]] = true;
            blossom[base[match[v]]] = true;
            parent[v] = child;
            child = match[v];
            v = parent[match[v]];
        }
    }

    int findAugmentingPath(int root) {
        std::fill(used.begin(), used.end(), false);
        std::fill(parent.begin(), parent.end(), -1);
        for (int i = 0; i < n; i++) base[i] = i;

        q = std::queue<int>();
        q.push(root);
        used[root] = true;

        while (!q.empty()) {
            int v = q.front();
            q.pop();

            for (int u = 0; u < n; u++) {
                if (!adj[v][u] || base[v] == base[u] || match[v] == u) continue;

                if (u == root || (match[u] != -1 && parent[match[u]] != -1)) {
                    int curBase = lca(v, u);
                    std::fill(blossom.begin(), blossom.end(), false);
                    markPath(v, curBase, u);
                    markPath(u, curBase, v);

                    for (int i = 0; i < n; i++) {
                        if (!blossom[base[i]]) continue;
                        base[i] = curBase;
                        if (!used[i]) {
                            used[i] = true;
                            q.push(i);
                        }
                    }
                } else if (parent[u] == -1) {
                    parent[u] = v;
                    if (match[u] == -1)
                        return u;

                    int next = match[u];
                    used[next] = true;
                    q.push(next);
                }
            }
        }

        return -1;
    }
};

static int maxMatching(const std::vector<std::vector<int>>& adj, int n,
                       std::vector<int>& match) {
    return GeneralMatching(adj, n).run(match);
}

EdgeCoverResult minEdgeCover(const Graph& g,
                              const std::vector<std::vector<double>>& W,
                              bool useMST,
                              const std::vector<MSTEdge>& mstEdges) {
    const int n = g.n;

    // Build undirected adjacency matrix for the chosen graph
    std::vector<std::vector<int>>    adjM(n, std::vector<int>(n, 0));
    std::vector<std::vector<double>> wM(n, std::vector<double>(n, 0.0));

    if (useMST) {
        for (auto& e : mstEdges) {
            adjM[e.u][e.v] = adjM[e.v][e.u] = 1;
            wM[e.u][e.v]   = wM[e.v][e.u]   = e.weight;
        }
    } else {
        for (int i = 0; i < n; i++)
            for (int j = i + 1; j < n; j++) {
                bool edge = g.hasEdge(i, j) || g.hasEdge(j, i);
                if (!edge) continue;
                adjM[i][j] = adjM[j][i] = 1;
                double w = W[i][j] < SHIMBELL_INF / 2 ? W[i][j]
                         : (W[j][i] < SHIMBELL_INF / 2 ? W[j][i] : 0.0);
                wM[i][j] = wM[j][i] = w;
            }
    }

    // Find maximum matching
    std::vector<int> match;
    int ms = maxMatching(adjM, n, match);

    // Build edge cover: start with matching edges
    std::vector<bool> covered(n, false);
    std::vector<MSTEdge> cover;

    for (int u = 0; u < n; u++) {
        if (match[u] != -1 && u < match[u]) {
            cover.push_back({ u, match[u], wM[u][match[u]] });
            covered[u] = covered[match[u]] = true;
        }
    }

    // For each unmatched vertex, add any incident edge
    for (int u = 0; u < n; u++) {
        if (covered[u]) continue;
        bool found = false;
        for (int v = 0; v < n && !found; v++) {
            if (adjM[u][v]) {
                cover.push_back({ u, v, wM[u][v] });
                covered[u] = true;
                found = true;
            }
        }
        if (!found)
            std::cout << "  Warning: vertex " << u + 1
                      << " is isolated — cannot be covered.\n";
    }

    return { cover, ms };
}

void printEdgeCoverResult(const EdgeCoverResult& res) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  MINIMUM EDGE COVER\n";
    std::cout << "  " << sep << "\n";
    std::cout << "\n  Maximum matching  |M| = " << res.matchingSize << "\n";
    std::cout << "  Min edge cover   |C| = " << res.cover.size()
              << "  (= n - |M| when no isolated vertices)\n";
    std::cout << "\n  Cover edges:\n";
    std::cout << "    Arc        weight\n";
    std::cout << "    " << std::string(22, '-') << "\n";
    for (auto& e : res.cover)
        std::cout << "    (" << e.u + 1 << " -- " << e.v + 1
                  << ")    " << e.weight << "\n";
    std::cout << "\n  " << sep << "\n";
}
