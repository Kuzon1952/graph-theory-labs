#include "graph.h"
#include "constants.h"  // SHIMBELL_INF, SHIMBELL_NINF
#include <iomanip>
#include <string>

static inline int D(int v) { return v + 1; }

Graph::Graph(int n, bool directed)
    : n(n), directed(directed), adj(n, std::vector<int>(n, 0)) {}

void Graph::addEdge(int u, int v) {
    adj[u][v] = 1;
    if (!directed)
        adj[v][u] = 1;
}

bool Graph::hasEdge(int u, int v) const {
    return adj[u][v] == 1;
}

std::vector<int> Graph::neighbors(int u) const {
    std::vector<int> result;
    for (int v = 0; v < n; v++)
        if (adj[u][v] == 1)
            result.push_back(v);
    return result;
}

// ── Just the adjacency matrix ─────────────────────────────────────────────────
void Graph::printAdjMatrix() const {
    const std::string type = directed ? "DIRECTED" : "UNDIRECTED";
    const std::string sep(60, '-');

    std::cout << "\n  " << sep << "\n";
    std::cout << "  ADJACENCY MATRIX  [" << type << "]\n";
    std::cout << "  " << sep << "\n\n";

    std::cout << "       ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(4) << D(j);
    std::cout << "\n  " << std::string(5 + n * 4, '-') << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(4) << adj[i][j];
        std::cout << "\n";
    }

    if (!directed)
        std::cout << "\n  Note: Undirected => matrix is symmetric (mat[i][j] == mat[j][i])\n";
    std::cout << "\n  " << sep << "\n";
}

// ── Full view ─────────────────────────────────────────────────────────────────
void Graph::printFull(const std::vector<std::vector<double>>* W) const {
    const std::string type = directed ? "DIRECTED" : "UNDIRECTED";
    const std::string sep(60, '-');

    // ── Adjacency List ──────────────────────────────────────
    std::cout << "\n  " << sep << "\n";
    std::cout << "  ADJACENCY LIST  [" << type << "]\n";
    std::cout << "  " << sep << "\n\n";

    for (int u = 0; u < n; u++) {
        auto nb = neighbors(u);
        std::cout << "  Vertex " << std::setw(3) << D(u) << "  -->  ";
        if (nb.empty()) {
            std::cout << "(isolated)";
        } else {
            for (int i = 0; i < (int)nb.size(); i++) {
                if (i) std::cout << ", ";
                std::cout << D(nb[i]);
            }
        }
        std::cout << "   (degree=" << nb.size() << ")\n";
    }

    // ── Edge List ─────────────────────────────────────────────
    std::cout << "\n  EDGE LIST:\n";
    const std::string arrow = directed ? "  ---->  " : "  ----  ";
    for (int u = 0; u < n; u++)
        for (int v = directed ? 0 : u + 1; v < n; v++)
            if (adj[u][v])
                std::cout << "    " << std::setw(3) << D(u)
                          << arrow << std::setw(3) << D(v) << "\n";

    // ── Adjacency Matrix ──────────────────────────────────────
    std::cout << "\n  " << sep << "\n";
    std::cout << "  ADJACENCY MATRIX  [" << type << "]\n";
    std::cout << "  " << sep << "\n\n";

    std::cout << "       ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(4) << D(j);
    std::cout << "\n  " << std::string(5 + n * 4, '-') << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << D(i) << " |";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(4) << adj[i][j];
        std::cout << "\n";
    }
    if (!directed)
        std::cout << "\n  Note: Undirected => matrix is symmetric (mat[i][j] == mat[j][i])\n";

    // ── Weight Matrix (if available) ──────────────────────────
    if (W) {
        std::cout << "\n  " << sep << "\n";
        std::cout << "  WEIGHT MATRIX  (INF = no edge, 0 = self)\n";
        std::cout << "  " << sep << "\n\n";

        const int CW = 8;
        std::cout << "       ";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(CW) << D(j);
        std::cout << "\n  " << std::string(5 + n * CW, '-') << "\n";

        for (int i = 0; i < n; i++) {
            std::cout << "  " << std::setw(3) << D(i) << " |";
            for (int j = 0; j < n; j++) {
                double v = (*W)[i][j];
                if (v >= SHIMBELL_INF / 2)
                    std::cout << std::setw(CW) << "INF";
                else if (v <= SHIMBELL_NINF / 2)
                    std::cout << std::setw(CW) << "-INF";
                else
                    std::cout << std::setw(CW) << static_cast<long long>(v);
            }
            std::cout << "\n";
        }
    }

    std::cout << "\n  " << sep << "\n";
}
