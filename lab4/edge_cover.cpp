#include "edge_cover.h"
#include "constants.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <queue>
#include <algorithm>

// BFS-based maximum matching (augmenting paths).
// Works correctly for bipartite graphs (trees are always bipartite).
// For general graphs it is a maximal matching (may not be maximum for odd cycles).
static int maxMatching(const std::vector<std::vector<int>>& adj, int n,
                       std::vector<int>& match) {
    match.assign(n, -1);
    int result = 0;

    for (int start = 0; start < n; start++) {
        if (match[start] != -1) continue;

        // BFS to find an augmenting path from 'start'
        std::vector<int> prev(n, -2);   // -2 = unvisited
        prev[start] = -1;
        std::queue<int> q;
        q.push(start);
        int endpoint = -1;

        while (!q.empty() && endpoint == -1) {
            int u = q.front(); q.pop();
            for (int v = 0; v < n; v++) {
                if (!adj[u][v] || prev[v] != -2) continue;
                prev[v] = u;
                if (match[v] == -1) {
                    endpoint = v;
                    break;
                }
                // Go through matched partner of v
                int w = match[v];
                prev[w] = v;
                q.push(w);
            }
        }

        if (endpoint == -1) continue;

        // Augment along found path (check v != -1 before accessing prev[v])
        int v = endpoint;
        while (v != -1 && prev[v] != -1) {
            int u  = prev[v];
            int pu = prev[u];    // may be -1 at start vertex
            match[v] = u;
            match[u] = v;
            v = pu;
        }
        result++;
    }
    return result;
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
    std::cout << "\n  Maximum matching size: " << res.matchingSize << "\n";
    std::cout << "  Edge cover size:       " << res.cover.size() << "\n";
    std::cout << "\n  Cover edges:\n";
    for (auto& e : res.cover)
        std::cout << "    (" << e.u + 1 << ", " << e.v + 1
                  << ")  weight=" << e.weight << "\n";
    std::cout << "\n  " << sep << "\n";
}
