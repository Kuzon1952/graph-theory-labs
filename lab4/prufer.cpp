#include "prufer.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <map>
#include <algorithm>

PruferResult pruferEncode(const std::vector<MSTEdge>& mstEdges, int n) {
    // Build adjacency for the tree: adj[u] = set of (neighbor, weight)
    std::vector<std::map<int, double>> adj(n);
    for (auto& e : mstEdges) {
        adj[e.u][e.v] = e.weight;
        adj[e.v][e.u] = e.weight;
    }

    // Degree array
    std::vector<int> deg(n, 0);
    for (int i = 0; i < n; i++) deg[i] = (int)adj[i].size();

    // Min-heap of leaves (vertices with degree 1)
    std::set<int> leaves;
    for (int i = 0; i < n; i++)
        if (deg[i] == 1) leaves.insert(i);

    std::vector<int>    code;
    std::vector<double> weights;

    // n-2 iterations produce the Prüfer code
    int remaining = n;
    while (remaining > 2) {
        int leaf = *leaves.begin();
        leaves.erase(leaves.begin());

        // The single neighbor of leaf
        auto it = adj[leaf].begin();
        int   neighbor = it->first;
        double w       = it->second;

        code.push_back(neighbor + 1);   // store 1-based
        weights.push_back(w);

        std::cout << "    Remove leaf " << leaf + 1
                  << "  neighbor=" << neighbor + 1
                  << "  weight=" << w << "\n";

        // Remove leaf from tree
        adj[leaf].clear();
        adj[neighbor].erase(leaf);
        deg[leaf] = 0;
        deg[neighbor]--;
        if (deg[neighbor] == 1) leaves.insert(neighbor);

        remaining--;
    }

    // The last edge (between the two remaining vertices)
    std::vector<int> last;
    for (int i = 0; i < n; i++)
        if (!adj[i].empty()) last.push_back(i);

    if (last.size() == 2) {
        double w = adj[last[0]][last[1]];
        weights.push_back(w);
        std::cout << "    Last edge: (" << last[0] + 1 << ", "
                  << last[1] + 1 << ")  weight=" << w << "\n";
    }

    return { code, weights };
}

std::vector<MSTEdge> pruferDecode(const std::vector<int>&    code,
                                   const std::vector<double>& weights,
                                   int n) {
    // Degree[v] = 1 + count(v in code), then degree for vertices not in code = 1
    std::vector<int> deg(n, 1);
    for (int x : code) deg[x - 1]++;   // code is 1-based

    // Min-set of leaves (degree 1)
    std::set<int> leaves;
    for (int i = 0; i < n; i++)
        if (deg[i] == 1) leaves.insert(i);

    std::vector<MSTEdge> edges;
    int wi = 0;

    for (int i = 0; i < (int)code.size(); i++) {
        int leaf     = *leaves.begin();
        leaves.erase(leaves.begin());
        int parent   = code[i] - 1;   // convert to 0-based
        double w     = weights[wi++];

        edges.push_back({ leaf, parent, w });
        std::cout << "    Connect " << leaf + 1 << " -- " << parent + 1
                  << "  weight=" << w << "\n";

        deg[leaf]--;
        deg[parent]--;
        if (deg[parent] == 1) leaves.insert(parent);
    }

    // Connect the two remaining vertices with the last stored weight
    std::vector<int> rem;
    for (int i = 0; i < n; i++)
        if (deg[i] == 1) rem.push_back(i);

    if (rem.size() == 2) {
        double w = weights[wi];
        edges.push_back({ rem[0], rem[1], w });
        std::cout << "    Connect " << rem[0] + 1 << " -- " << rem[1] + 1
                  << "  weight=" << w << "  (final edge)\n";
    }

    return edges;
}

void printPruferResult(const PruferResult& res, int n) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  PRUFER CODE  (n=" << n << ", code length=" << res.code.size() << ")\n";
    std::cout << "  " << sep << "\n";
    std::cout << "\n  Prufer sequence:  [ ";
    for (int i = 0; i < (int)res.code.size(); i++) {
        if (i) std::cout << ", ";
        std::cout << res.code[i];
    }
    std::cout << " ]\n";
    std::cout << "\n  Edge weights (in removal order, length " << res.weights.size() << "):\n  [ ";
    for (int i = 0; i < (int)res.weights.size(); i++) {
        if (i) std::cout << ", ";
        std::cout << res.weights[i];
    }
    std::cout << " ]\n";
    std::cout << "  (last weight is for the final edge not in Prufer sequence)\n";
    std::cout << "\n  " << sep << "\n";
}

void printDecodedTree(const std::vector<MSTEdge>& edges) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  DECODED TREE EDGES\n";
    std::cout << "  " << sep << "\n";
    double total = 0.0;
    for (auto& e : edges) {
        std::cout << "    (" << e.u + 1 << ", " << e.v + 1
                  << ")  weight=" << e.weight << "\n";
        total += e.weight;
    }
    std::cout << "\n  Total weight = " << total << "\n";
    std::cout << "\n  " << sep << "\n";
}
