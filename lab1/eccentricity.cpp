#include "eccentricity.h"
#include <queue>
#include <algorithm>
#include <iostream>
#include <iomanip>

static inline int D(int v) { return v + 1; }

std::vector<int> bfs(const Graph& g, int src) {
    std::vector<int> dist(g.n, -1);
    std::queue<int> q;
    dist[src] = 0;
    q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : g.neighbors(u)) {
            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                q.push(v);
            }
        }
    }
    return dist;
}

GraphMetrics computeMetrics(const Graph& g) {
    GraphMetrics m;
    m.eccentricities.resize(g.n, 0);
    m.distMatrix.resize(g.n);

    for (int v = 0; v < g.n; v++) {
        m.distMatrix[v] = bfs(g, v);
        int maxDist = 0;
        for (int u = 0; u < g.n; u++) {
            if (u != v && m.distMatrix[v][u] != -1)
                maxDist = std::max(maxDist, m.distMatrix[v][u]);
        }
        m.eccentricities[v] = maxDist;
    }

    m.diameter = *std::max_element(m.eccentricities.begin(), m.eccentricities.end());

    m.radius = m.diameter;
    for (int v = 0; v < g.n; v++)
        if (m.eccentricities[v] > 0)
            m.radius = std::min(m.radius, m.eccentricities[v]);
    if (m.diameter == 0) m.radius = 0;

    for (int v = 0; v < g.n; v++) {
        if (m.eccentricities[v] == m.radius && m.radius > 0)
            m.center.push_back(v);
        if (m.eccentricities[v] == m.diameter && m.diameter > 0)
            m.diametralVertices.push_back(v);
    }

    return m;
}

void printMetrics(const GraphMetrics& m) {
    int n = static_cast<int>(m.eccentricities.size());

    // ── Distance matrix ───────────────────────────────────────
    std::cout << "\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(5) << D(j);
    std::cout << "\n  " << std::string(5 + n * 5, '-') << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++) {
            int d = m.distMatrix[i][j];
            if (d == -1)
                std::cout << std::setw(5) << "inf";
            else
                std::cout << std::setw(5) << d;
        }
        std::cout << "\n";
    }

    // ── Eccentricity table ────────────────────────────────────
    std::cout << "\n";
    std::cout << "  Vertex  | Eccentricity | Role\n";
    std::cout << "  --------+--------------+--------------------------\n";
    for (int v = 0; v < n; v++) {
        int e = m.eccentricities[v];
        std::string role = "";
        if (m.diameter > 0 && e == m.diameter) role = "DIAMETRICAL";
        if (m.radius  > 0 && e == m.radius)   {
            role = (role.empty() ? "" : role + " + ");
            role += "CENTER";
        }
        if (role.empty()) role = "-";
        std::cout << "  " << std::setw(6) << D(v)
                  << "  |  " << std::setw(8) << e
                  << "    | " << role << "\n";
    }

    // ── Summary ───────────────────────────────────────────────
    std::cout << "\n  Radius   = " << m.radius   << "\n";
    std::cout << "  Diameter = " << m.diameter << "\n";

    std::cout << "\n  CENTER vertices      = { ";
    for (int v : m.center) std::cout << D(v) << " ";
    std::cout << "}\n";

    std::cout << "  DIAMETRICAL vertices = { ";
    for (int v : m.diametralVertices) std::cout << D(v) << " ";
    std::cout << "}\n";
}
