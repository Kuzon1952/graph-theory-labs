#include "eulerian.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <sstream>

static inline int D(int v) { return v + 1; }

static int degreeOf(const std::vector<std::vector<int>>& mult, int v) {
    int deg = 0;
    for (int u = 0; u < (int)mult.size(); u++)
        deg += mult[v][u];
    return deg;
}

static std::vector<int> degreesOf(const std::vector<std::vector<int>>& mult) {
    std::vector<int> deg(mult.size(), 0);
    for (int v = 0; v < (int)mult.size(); v++)
        deg[v] = degreeOf(mult, v);
    return deg;
}

static std::vector<int> oddVertices(const std::vector<int>& deg) {
    std::vector<int> odd;
    for (int i = 0; i < (int)deg.size(); i++)
        if (deg[i] % 2 != 0)
            odd.push_back(i);
    return odd;
}

static std::vector<int> bfsReachable(const std::vector<std::vector<int>>& mult,
                                     int start) {
    const int n = (int)mult.size();
    std::vector<int> visited(n, 0);
    std::queue<int> q;
    visited[start] = 1;
    q.push(start);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int u = 0; u < n; u++) {
            if (mult[v][u] > 0 && !visited[u]) {
                visited[u] = 1;
                q.push(u);
            }
        }
    }

    return visited;
}

static std::vector<std::vector<int>> connectedComponents(
        const std::vector<std::vector<int>>& mult) {
    const int n = (int)mult.size();
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
                if (mult[v][u] > 0 && !used[u]) {
                    used[u] = 1;
                    q.push(u);
                }
            }
        }
        comps.push_back(comp);
    }

    return comps;
}

static bool isConnected(const std::vector<std::vector<int>>& mult) {
    const int n = (int)mult.size();
    if (n <= 1) return true;
    auto seen = bfsReachable(mult, 0);
    return std::all_of(seen.begin(), seen.end(), [](int x) { return x != 0; });
}

static std::vector<int> shortestPath(const std::vector<std::vector<int>>& mult,
                                     int src, int dst) {
    const int n = (int)mult.size();
    std::vector<int> prev(n, -1);
    std::queue<int> q;
    prev[src] = src;
    q.push(src);

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        if (v == dst) break;
        for (int u = 0; u < n; u++) {
            if (mult[v][u] > 0 && prev[u] == -1) {
                prev[u] = v;
                q.push(u);
            }
        }
    }

    if (prev[dst] == -1) return {};
    std::vector<int> path;
    for (int v = dst; v != src; v = prev[v])
        path.push_back(v);
    path.push_back(src);
    std::reverse(path.begin(), path.end());
    return path;
}

static void addEdge(std::vector<std::vector<int>>& mult, int u, int v) {
    mult[u][v]++;
    mult[v][u]++;
}

static std::vector<int> hierholzer(std::vector<std::vector<int>> mult) {
    const int n = (int)mult.size();
    int start = 0;
    for (int i = 0; i < n; i++) {
        if (degreeOf(mult, i) > 0) {
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
            if (mult[v][u] > 0) {
                next = u;
                break;
            }
        }

        if (next == -1) {
            cycle.push_back(v);
            stack.pop_back();
        } else {
            mult[v][next]--;
            mult[next][v]--;
            stack.push_back(next);
        }
    }

    std::reverse(cycle.begin(), cycle.end());
    return cycle;
}

EulerianResult buildEulerianCycle(const Graph& g) {
    const int n = g.n;
    std::vector<std::vector<int>> mult(n, std::vector<int>(n, 0));

    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            if (g.hasEdge(i, j) || g.hasEdge(j, i))
                addEdge(mult, i, j);

    EulerianResult res;
    res.originalConnected = isConnected(mult);
    res.originalDegrees = degreesOf(mult);
    res.originalOddVertices = oddVertices(res.originalDegrees);
    res.originalEulerian = res.originalConnected && res.originalOddVertices.empty();

    if (!res.originalConnected) {
        auto comps = connectedComponents(mult);
        std::ostringstream os;
        os << "Graph is disconnected: " << comps.size()
           << " connected components found.";
        res.log.push_back(os.str());

        for (int i = 0; i + 1 < (int)comps.size(); i++) {
            int u = comps[i][0];
            int v = comps[i + 1][0];
            addEdge(mult, u, v);
            res.addedEdges.push_back({u, v});
            std::ostringstream add;
            add << "Added edge (" << D(u) << " -- " << D(v)
                << ") to connect components.";
            res.log.push_back(add.str());
        }
    }

    auto currentDegrees = degreesOf(mult);
    auto odd = oddVertices(currentDegrees);
    if (odd.empty()) {
        res.log.push_back("All vertex degrees are even; no parity modification needed.");
    } else {
        std::ostringstream os;
        os << "Odd vertices paired for modification:";
        for (int v : odd) os << " " << D(v);
        res.log.push_back(os.str());

        for (int i = 0; i + 1 < (int)odd.size(); i += 2) {
            int src = odd[i];
            int dst = odd[i + 1];
            auto path = shortestPath(mult, src, dst);
            if (path.empty()) continue;

            std::ostringstream p;
            p << "Duplicated path " << D(src) << " -> " << D(dst) << ": ";
            for (int k = 0; k < (int)path.size(); k++) {
                if (k) p << " -- ";
                p << D(path[k]);
            }
            res.log.push_back(p.str());

            for (int k = 0; k + 1 < (int)path.size(); k++) {
                int u = path[k];
                int v = path[k + 1];
                addEdge(mult, u, v);
                res.addedEdges.push_back({u, v});
                std::ostringstream add;
                add << "Added duplicate edge (" << D(u) << " -- " << D(v)
                    << ").";
                res.log.push_back(add.str());
            }
        }
    }

    res.modifiedDegrees = degreesOf(mult);
    res.eulerCycle = hierholzer(mult);
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

    std::cout << "\n  Modified degrees:\n";
    for (int i = 0; i < n; i++) {
        std::cout << "    deg(" << D(i) << ") = " << res.modifiedDegrees[i]
                  << "  " << (res.modifiedDegrees[i] % 2 ? "odd" : "even")
                  << "\n";
    }

    std::cout << "\n  Euler cycle:\n    ";
    for (int i = 0; i < (int)res.eulerCycle.size(); i++) {
        if (i) std::cout << " -> ";
        std::cout << D(res.eulerCycle[i]);
    }
    std::cout << "\n\n  " << sep << "\n";
}
