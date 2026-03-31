#include "tree_generator.h"
#include "distribution.h"
#include <random>
#include <vector>
#include <numeric>
#include <algorithm>

static std::mt19937& getRng() {
    static std::mt19937 rng(std::random_device{}());
    return rng;
}

// ─────────────────────────────────────────────────────────────────────────────
//  UNDIRECTED: connected acyclic graph (= tree)
//  Algorithm: degree-proportional random attachment
//
//  1. Sample degree sequence from Normal distribution; adjust sum to 2*(n-1).
//  2. Keep a "capacity" counter per vertex = desired degree.
//  3. Insert vertices one-by-one; choose parent ∝ remaining capacity.
//  Guaranteed: connected + acyclic (new vertex always attaches to existing tree).
// ─────────────────────────────────────────────────────────────────────────────
static Graph buildUndirectedTree(int n) {
    Graph g(n, false);
    if (n <= 1) return g;
    if (n == 2) { g.addEdge(0, 1); return g; }

    std::vector<int> capacity = generateDegreeSequence(n);
    auto& rng = getRng();

    std::vector<int> inTree;
    inTree.reserve(n);
    inTree.push_back(0);

    std::vector<int> order(n - 1);
    std::iota(order.begin(), order.end(), 1);
    std::shuffle(order.begin(), order.end(), rng);

    for (int newV : order) {
        int totalCap = 0;
        for (int v : inTree) totalCap += std::max(0, capacity[v]);

        int parent;
        if (totalCap <= 0) {
            std::uniform_int_distribution<int> upick(0, (int)inTree.size() - 1);
            parent = inTree[upick(rng)];
        } else {
            std::uniform_int_distribution<int> roll(1, totalCap);
            int r = roll(rng), cum = 0;
            parent = inTree[0];
            for (int v : inTree) {
                cum += std::max(0, capacity[v]);
                if (r <= cum) { parent = v; break; }
            }
        }

        g.addEdge(parent, newV);
        capacity[parent]--;
        capacity[newV]--;
        inTree.push_back(newV);
    }

    return g;
}

// ─────────────────────────────────────────────────────────────────────────────
//  DIRECTED:
//  Algorithm:
//    1. Create a random topological ordering  [0], [1], ..., [n-1].
//       Any edge must go from a lower to a higher position in this ordering
//        acyclicity is guaranteed by construction.
//    2. Add the spanning path  [0] to [1]...[n-1]  for weak connectivity.
//    3. For each vertex π[i], sample an extra out-degree from the Normal
//       distribution and add that many additional forward edges to randomly
//       chosen [j]  (j > i).  This produces a non-tree DAG whose edge count
//       and degree distribution are driven by the Normal distribution.
// ─────────────────────────────────────────────────────────────────────────────
static Graph buildDirectedDAG(int n) {
    Graph g(n, true);
    if (n <= 1) return g;
    if (n == 2) { g.addEdge(0, 1); return g; }

    auto& rng = getRng();

    // Step 1: random topological ordering
    std::vector<int> topo(n);
    std::iota(topo.begin(), topo.end(), 0);
    std::shuffle(topo.begin(), topo.end(), rng);

    // Step 2: spanning path  topo[0]→topo[1]→...→topo[n-1]
    for (int i = 0; i < n - 1; i++)
        g.addEdge(topo[i], topo[i + 1]);

    // Step 3: extra forward edges driven by Normal distribution.

    for (int i = 0; i < n - 2; i++) {          // last vertex has no forward targets
        int u = topo[i];

        // Extra out-degree from Normal distribution (|sample| mapped to [0, gap])
        int gap       = n - i - 2;              // number of available forward targets
        double raw    = std::abs(normalSample(12));
        int extraDeg  = std::min((int)std::round(raw), gap);

        // Collect candidate targets (all j > i+1 to avoid duplicating path edge)
        std::vector<int> candidates;
        candidates.reserve(gap);
        for (int j = i + 2; j < n; j++)
            candidates.push_back(j);
        std::shuffle(candidates.begin(), candidates.end(), rng);

        int added = 0;
        for (int j : candidates) {
            if (added >= extraDeg) break;
            int v = topo[j];
            if (!g.hasEdge(u, v)) {
                g.addEdge(u, v);
                added++;
            }
        }
    }

    return g;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public entry point
// ─────────────────────────────────────────────────────────────────────────────
Graph generateTree(int n, bool directed) {
    if (directed) return buildDirectedDAG(n);
    else          return buildUndirectedTree(n);
}
