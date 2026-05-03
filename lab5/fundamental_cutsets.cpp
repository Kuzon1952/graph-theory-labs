#include "fundamental_cutsets.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <set>

static inline int D(int v) { return v + 1; }

static std::vector<CutsetEdge> graphEdges(const Graph& g) {
    std::vector<CutsetEdge> edges;
    for (int i = 0; i < g.n; i++)
        for (int j = i + 1; j < g.n; j++)
            if (g.hasEdge(i, j) || g.hasEdge(j, i))
                edges.push_back({i, j});
    return edges;
}

static std::vector<int> treeComponentAfterRemoving(
        int n,
        const std::vector<MSTEdge>& tree,
        int removedIndex,
        int start) {
    std::vector<std::vector<int>> adj(n);
    for (int i = 0; i < (int)tree.size(); i++) {
        if (i == removedIndex) continue;
        int u = tree[i].u;
        int v = tree[i].v;
        adj[u].push_back(v);
        adj[v].push_back(u);
    }

    std::vector<int> inComponent(n, 0);
    std::queue<int> q;
    q.push(start);
    inComponent[start] = 1;

    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int u : adj[v]) {
            if (!inComponent[u]) {
                inComponent[u] = 1;
                q.push(u);
            }
        }
    }

    return inComponent;
}

std::vector<FundamentalCutset> buildFundamentalCutsets(
        const Graph& g,
        const std::vector<MSTEdge>& mstEdges) {
    std::vector<FundamentalCutset> result;
    auto edges = graphEdges(g);

    for (int i = 0; i < (int)mstEdges.size(); i++) {
        const auto& treeEdge = mstEdges[i];
        auto left = treeComponentAfterRemoving(g.n, mstEdges, i, treeEdge.u);

        FundamentalCutset cut;
        cut.index = i + 1;
        cut.treeEdge = treeEdge;

        for (auto e : edges) {
            if (left[e.u] != left[e.v])
                cut.edges.push_back(e);
        }

        std::sort(cut.edges.begin(), cut.edges.end(),
                  [](const CutsetEdge& a, const CutsetEdge& b) {
                      if (a.u != b.u) return a.u < b.u;
                      return a.v < b.v;
                  });

        result.push_back(cut);
    }

    return result;
}

void printFundamentalCutsets(const std::vector<FundamentalCutset>& cutsets) {
    const std::string sep(60, '-');
    std::cout << "\n  " << sep << "\n";
    std::cout << "  LAB 5: FUNDAMENTAL CUTSET SYSTEM (EVEN VARIANT)\n";
    std::cout << "  " << sep << "\n\n";

    for (const auto& cut : cutsets) {
        std::cout << "  Cutset " << cut.index
                  << "  for tree edge (" << D(cut.treeEdge.u)
                  << " -- " << D(cut.treeEdge.v) << "):\n";
        std::cout << "    { ";
        for (int i = 0; i < (int)cut.edges.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << "(" << D(cut.edges[i].u)
                      << "--" << D(cut.edges[i].v) << ")";
        }
        std::cout << " }\n";
    }

    std::cout << "\n  " << sep << "\n";
}

void printCutsetSymmetricDifference(
        const std::vector<FundamentalCutset>& cutsets,
        const std::vector<int>& selectedIndices) {
    std::set<std::pair<int, int>> result;

    for (int idx : selectedIndices) {
        if (idx < 1 || idx > (int)cutsets.size()) continue;
        for (auto e : cutsets[idx - 1].edges) {
            auto key = std::make_pair(e.u, e.v);
            auto it = result.find(key);
            if (it == result.end())
                result.insert(key);
            else
                result.erase(it);
        }
    }

    std::cout << "\n  Symmetric difference of selected cutsets";
    if (!selectedIndices.empty()) {
        std::cout << " (";
        for (int i = 0; i < (int)selectedIndices.size(); i++) {
            if (i) std::cout << ", ";
            std::cout << selectedIndices[i];
        }
        std::cout << ")";
    }
    std::cout << ":\n";

    std::cout << "    { ";
    int printed = 0;
    for (auto [u, v] : result) {
        if (printed++) std::cout << ", ";
        std::cout << "(" << D(u) << "--" << D(v) << ")";
    }
    std::cout << " }\n";
}
