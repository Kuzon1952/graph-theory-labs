#include "eulerian.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>

static inline int D(int v) { return v + 1; }

static int degreeOf(const std::vector<std::vector<int>>& adj, int v) {
    int deg = 0;
    for (int u = 0; u < (int)adj.size(); u++)
        deg += adj[v][u];
    return deg;
}

static std::vector<int> degreesOf(const std::vector<std::vector<int>>& adj) {
    std::vector<int> deg(adj.size(), 0);
    for (int v = 0; v < (int)adj.size(); v++)
        deg[v] = degreeOf(adj, v);
    return deg;
}

static std::vector<int> oddVertices(const std::vector<int>& deg) {
    std::vector<int> odd;
    for (int i = 0; i < (int)deg.size(); i++)
        if (deg[i] % 2 != 0)
            odd.push_back(i);
    return odd;
}

static bool hasAnyEdge(const std::vector<std::vector<int>>& adj) {
    for (int i = 0; i < (int)adj.size(); i++)
        for (int j = i + 1; j < (int)adj.size(); j++)
            if (adj[i][j] > 0)
                return true;
    return false;
}

static std::vector<int> bfsReachable(const std::vector<std::vector<int>>& adj,
                                     int start) {
    const int n = (int)adj.size();
    std::vector<int> visited(n, 0);
    std::queue<int> q;
    visited[start] = 1;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int u = 0; u < n; u++) {
            if (adj[v][u] > 0 && !visited[u]) {
                visited[u] = 1;
                q.push(u);
            }
        }
    }

    return visited;
}

static std::vector<std::vector<int>> connectedComponents(
        const std::vector<std::vector<int>>& adj) {
    const int n = (int)adj.size();
    std::vector<int> used(n, 0);
    std::vector<std::vector<int>> comps;

    for (int start = 0; start < n; start++) {
        if (used[start]) continue;

        std::vector<int> comp;
        std::queue<int> q;
        q.push(start);
        used[start] = 1;

        while (!q.empty()) {
            int v = q.front();
            q.pop();
            comp.push_back(v);
            for (int u = 0; u < n; u++) {
                if (adj[v][u] > 0 && !used[u]) {
                    used[u] = 1;
                    q.push(u);
                }
            }
        }
        comps.push_back(comp);
    }

    return comps;
}

static bool isConnected(const std::vector<std::vector<int>>& adj) {
    const int n = (int)adj.size();
    if (n <= 1) return true;
    auto seen = bfsReachable(adj, 0);
    return std::all_of(seen.begin(), seen.end(), [](int x) { return x != 0; });
}

static void addSimpleEdge(std::vector<std::vector<int>>& adj, int u, int v) {
    adj[u][v] = 1;
    adj[v][u] = 1;
}

static void removeSimpleEdge(std::vector<std::vector<int>>& adj, int u, int v) {
    adj[u][v] = 0;
    adj[v][u] = 0;
}

static bool canRemoveWithoutDisconnecting(
        const std::vector<std::vector<int>>& adj, int u, int v) {
    if (adj[u][v] == 0) return false;
    auto test = adj;
    removeSimpleEdge(test, u, v);
    return isConnected(test);
}

static bool tryEdgeSwapForOddPair(
        std::vector<std::vector<int>>& adj,
        int u,
        int v,
        EulerianResult& res) {
    const int n = (int)adj.size();

    for (int x = 0; x < n; x++) {
        if (x == u || x == v) continue;

        if (adj[u][x] > 0 && adj[v][x] == 0) {
            addSimpleEdge(adj, v, x);
            if (canRemoveWithoutDisconnecting(adj, u, x)) {
                removeSimpleEdge(adj, u, x);
                res.addedEdges.push_back({v, x});
                res.removedEdges.push_back({u, x});

                std::ostringstream add;
                add << "Added missing edge (" << D(v) << " -- " << D(x)
                    << ").";
                res.log.push_back(add.str());

                std::ostringstream rem;
                rem << "Removed edge (" << D(u) << " -- " << D(x)
                    << "); graph remains connected.";
                res.log.push_back(rem.str());
                return true;
            }
            removeSimpleEdge(adj, v, x);
        }

        if (adj[v][x] > 0 && adj[u][x] == 0) {
            addSimpleEdge(adj, u, x);
            if (canRemoveWithoutDisconnecting(adj, v, x)) {
                removeSimpleEdge(adj, v, x);
                res.addedEdges.push_back({u, x});
                res.removedEdges.push_back({v, x});

                std::ostringstream add;
                add << "Added missing edge (" << D(u) << " -- " << D(x)
                    << ").";
                res.log.push_back(add.str());

                std::ostringstream rem;
                rem << "Removed edge (" << D(v) << " -- " << D(x)
                    << "); graph remains connected.";
                res.log.push_back(rem.str());
                return true;
            }
            removeSimpleEdge(adj, u, x);
        }
    }

    return false;
}

static std::vector<int> hierholzer(std::vector<std::vector<int>> adj) {
    const int n = (int)adj.size();
    int start = 0;
    for (int i = 0; i < n; i++) {
        if (degreeOf(adj, i) > 0) {
            start = i;
            break;
        }
    }

    std::vector<int> stack;
    std::vector<int> cycle;
    stack.push_back(start);

    while (!stack.empty()) {
        int v = stack.back();
        int next = -1;
        for (int u = 0; u < n; u++) {
            if (adj[v][u] > 0) {
                next = u;
                break;
            }
        }

        if (next == -1) {
            cycle.push_back(v);
            stack.pop_back();
        } else {
            adj[v][next] = 0;
            adj[next][v] = 0;
            stack.push_back(next);
        }
    }

    std::reverse(cycle.begin(), cycle.end());
    return cycle;
}

EulerianResult buildEulerianCycle(const Graph& g) {
    const int n = g.n;
    std::vector<std::vector<int>> adj(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (g.hasEdge(i, j) || g.hasEdge(j, i))
                addSimpleEdge(adj, i, j);

    EulerianResult res;
    res.originalConnected = isConnected(adj);
    res.originalDegrees = degreesOf(adj);
    res.originalOddVertices = oddVertices(res.originalDegrees);
    res.originalEulerian = res.originalConnected &&
                           res.originalOddVertices.empty() &&
                           hasAnyEdge(adj);
    res.modificationPossible = true;

    if (!hasAnyEdge(adj)) {
        res.log.push_back("Graph has no edges; Euler cycle is not constructed.");
        res.modificationPossible = false;
        res.modifiedDegrees = res.originalDegrees;
        return res;
    }

    if (!res.originalConnected) {
        auto comps = connectedComponents(adj);
        std::ostringstream os;
        os << "Graph is disconnected: " << comps.size()
           << " connected components found.";
        res.log.push_back(os.str());

        for (int i = 0; i + 1 < (int)comps.size(); i++) {
            int u = comps[i][0];
            int v = comps[i + 1][0];
            addSimpleEdge(adj, u, v);
            res.addedEdges.push_back({u, v});
            std::ostringstream add;
            add << "Added edge (" << D(u) << " -- " << D(v)
                << ") to connect components.";
            res.log.push_back(add.str());
        }
    }

    auto currentDegrees = degreesOf(adj);
    auto odd = oddVertices(currentDegrees);
    if (odd.empty()) {
        res.log.push_back("All vertex degrees are even; no parity modification needed.");
    } else {
        const int maxSteps = n * n + 1;
        for (int step = 0; step < maxSteps; step++) {
            currentDegrees = degreesOf(adj);
            odd = oddVertices(currentDegrees);
            if (odd.empty()) break;

            std::ostringstream os;
            os << "Odd vertices:";
            for (int v : odd) os << " " << D(v);
            res.log.push_back(os.str());

            if (n < 3) {
                res.log.push_back("Cannot modify this graph under restrictions: "
                                  "need at least 3 vertices to avoid duplicate edges "
                                  "and keep the graph connected.");
                res.modificationPossible = false;
                break;
            }

            bool changed = false;

            for (int i = 0; i < (int)odd.size() && !changed; i++) {
                for (int j = i + 1; j < (int)odd.size() && !changed; j++) {
                    int u = odd[i];
                    int v = odd[j];
                    if (adj[u][v] == 0) {
                        addSimpleEdge(adj, u, v);
                        res.addedEdges.push_back({u, v});
                        std::ostringstream add;
                        add << "Added missing edge (" << D(u) << " -- "
                            << D(v) << ").";
                        res.log.push_back(add.str());
                        changed = true;
                    }
                }
            }

            for (int i = 0; i < (int)odd.size() && !changed; i++) {
                for (int j = i + 1; j < (int)odd.size() && !changed; j++) {
                    int u = odd[i];
                    int v = odd[j];
                    if (adj[u][v] > 0 && canRemoveWithoutDisconnecting(adj, u, v)) {
                        removeSimpleEdge(adj, u, v);
                        res.removedEdges.push_back({u, v});
                        std::ostringstream rem;
                        rem << "Removed edge (" << D(u) << " -- "
                            << D(v) << "); graph remains connected.";
                        res.log.push_back(rem.str());
                        changed = true;
                    }
                }
            }

            for (int i = 0; i < (int)odd.size() && !changed; i++) {
                for (int j = i + 1; j < (int)odd.size() && !changed; j++) {
                    int u = odd[i];
                    int v = odd[j];
                    if (adj[u][v] > 0)
                        changed = tryEdgeSwapForOddPair(adj, u, v, res);
                }
            }

            if (!changed) {
                res.log.push_back("Cannot modify this graph under restrictions: "
                                  "no legal add/remove operation was found.");
                res.modificationPossible = false;
                break;
            }
        }

        currentDegrees = degreesOf(adj);
        odd = oddVertices(currentDegrees);
        if (!odd.empty() && res.modificationPossible) {
            res.log.push_back("Cannot modify this graph under restrictions: "
                              "the add/remove process did not reach all even degrees.");
            res.modificationPossible = false;
        }
    }

    res.modifiedDegrees = degreesOf(adj);
    if (res.modificationPossible && isConnected(adj) &&
        oddVertices(res.modifiedDegrees).empty()) {
        res.eulerCycle = hierholzer(adj);
    } else {
        res.eulerCycle.clear();
    }
    return res;
}

void printEulerianResult(const EulerianResult& res) {
    const std::string sep(60, '-');
    const int n = (int)res.originalDegrees.size();

    std::cout << "\n  " << sep << "\n";
    std::cout << "  LAB 5: EULERIAN GRAPH CHECK AND EULER CYCLE\n";
    std::cout << "  " << sep << "\n\n";

    std::cout << "  Connected: " << (res.originalConnected ? "YES" : "NO") << "\n";
    std::cout << "  Original graph is Eulerian: "
              << (res.originalEulerian ? "YES" : "NO") << "\n\n";

    std::cout << "  Original degrees:\n";
    std::cout << "    Vertex | Degree | Parity\n";
    std::cout << "    " << std::string(26, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "      " << std::setw(3) << D(i)
                  << "   |  " << std::setw(3) << res.originalDegrees[i]
                  << "   | " << (res.originalDegrees[i] % 2 ? "odd" : "even")
                  << "\n";
    }

    std::cout << "\n  Modification log:\n";
    for (const auto& line : res.log)
        std::cout << "    - " << line << "\n";

    if (!res.modificationPossible) {
        std::cout << "\n  " << sep << "\n";
        return;
    }

    std::cout << "\n  Modified degrees:\n";
    for (int i = 0; i < (int)res.modifiedDegrees.size(); i++) {
        std::cout << "    deg(" << D(i) << ") = " << res.modifiedDegrees[i]
                  << "  " << (res.modifiedDegrees[i] % 2 ? "odd" : "even")
                  << "\n";
    }

    std::cout << "\n  Euler cycle:\n    ";
    if (res.eulerCycle.empty()) {
        std::cout << "Not constructed.";
    } else {
        for (int i = 0; i < (int)res.eulerCycle.size(); i++) {
            if (i) std::cout << " -> ";
            std::cout << D(res.eulerCycle[i]);
        }
    }
    std::cout << "\n\n  " << sep << "\n";
}
