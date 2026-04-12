#include "dag_generator.h"
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
//  EXTRA_EDGE_SCALE: multiplier applied to the sampled out-degree for each
//  vertex in the directed DAG. Controls graph density.
//
//  1.0 = sparse    4.0 = medium    8.0 = dense (current)
// ─────────────────────────────────────────────────────────────────────────────
// NEW: separate scales
static constexpr double EXTRA_EDGE_SCALE_DIRECTED   = 8.0;  // dense DAG
static constexpr double EXTRA_EDGE_SCALE_UNDIRECTED = 1.5;  // sparse enough to vary after symmetrize

// ─────────────────────────────────────────────────────────────────────────────
//  OLD COMMENT (kept for reference):
//  UNDIRECTED: connected acyclic graph (= tree)
//  Algorithm: degree-proportional random attachment
//  1. Sample degree sequence from Normal distribution; adjust sum to 2*(n-1).
//  2. Keep a "capacity" counter per vertex = desired degree.
//  3. Insert vertices one-by-one; choose parent proportional to remaining capacity.
//  Guaranteed: connected + acyclic (new vertex always attaches to existing tree).
//
//  NEW: renamed from buildUndirectedTree() to buildUndirectedGraph().
//  The structure is still a tree (n vertices, n-1 edges, connected, acyclic)
//  but we avoid the name "tree" in the function to stay consistent with the
//  rename of the public entry point generateTree() -> generateDAG().
//  Variable renamed: inTree -> inGraph (tracks which vertices are already
//  in the graph being built).
// ─────────────────────────────────────────────────────────────────────────────
static Graph buildUndirectedGraph(int n) {
    Graph g(n, false);
    if (n <= 1) return g;
    if (n == 2) { g.addEdge(0, 1); return g; }

    std::vector<int> capacity = generateDegreeSequence(n);
    auto& rng = getRng();

    // OLD: std::vector<int> inTree;
    // NEW: renamed to inGraph — tracks vertices already added to the graph
    std::vector<int> inGraph;
    inGraph.reserve(n);
    inGraph.push_back(0);

    std::vector<int> order(n - 1);
    std::iota(order.begin(), order.end(), 1);
    std::shuffle(order.begin(), order.end(), rng);

    for (int newV : order) {
        int totalCap = 0;
        for (int v : inGraph) totalCap += std::max(0, capacity[v]);

        int parent;
        if (totalCap <= 0) {
            std::uniform_int_distribution<int> upick(0, (int)inGraph.size() - 1);
            parent = inGraph[upick(rng)];
        } else {
            std::uniform_int_distribution<int> roll(1, totalCap);
            int r = roll(rng), cum = 0;
            parent = inGraph[0];
            for (int v : inGraph) {
                cum += std::max(0, capacity[v]);
                if (r <= cum) { parent = v; break; }
            }
        }

        g.addEdge(parent, newV);
        capacity[parent]--;
        capacity[newV]--;
        inGraph.push_back(newV);
    }

    return g;
}

// ─────────────────────────────────────────────────────────────────────────────
//  DIRECTED: connected acyclic directed graph (DAG)
//  Algorithm — "generation by vertex degrees" as required by the lab:
//
//  1. Sample an OUT-DEGREE sequence from the Normal distribution via
//     generateDegreeSequence() — same function used for undirected.
//     Scale each degree by EXTRA_EDGE_SCALE for density; cap at available
//     forward targets so acyclicity is preserved.
//
//  2. Create a random topological ordering. Edges only go from lower to
//     higher position => acyclicity guaranteed by construction.
//
//  3. Spanning path topo[0]->topo[1]->...->topo[n-1] added first to
//     guarantee weak connectivity; counts against each vertex's degree budget.
//
//  4. Fill remaining out-degree budget per vertex with random forward edges.
// ─────────────────────────────────────────────────────────────────────────────
static Graph buildDirectedDAG(int n, double scale = EXTRA_EDGE_SCALE_DIRECTED) {
    Graph g(n, true);
    if (n <= 1) return g;
    if (n == 2) { g.addEdge(0, 1); return g; }

    auto& rng = getRng();

    // Step 1: random topological ordering
    std::vector<int> topo(n);
    std::iota(topo.begin(), topo.end(), 0);
    std::shuffle(topo.begin(), topo.end(), rng);

    // Step 2: sample out-degree for each vertex from Normal distribution,
    //         scaled by EXTRA_EDGE_SCALE, capped by available forward targets.
    std::vector<int> outDeg = generateDegreeSequence(n);
    for (int i = 0; i < n; i++) {
        int maxOut = n - i - 1;  // topo[i] can reach at most (n-i-1) vertices ahead
        outDeg[i] = std::min((int)std::round(outDeg[i] * scale), maxOut);
        outDeg[i] = std::max(outDeg[i], (i < n - 1) ? 1 : 0);  // at least 1 (except last)
    }

    // Step 3: spanning path for connectivity; consume one slot per vertex
    for (int i = 0; i < n - 1; i++) {
        g.addEdge(topo[i], topo[i + 1]);
        outDeg[i]--;
    }

    // Step 4: fill remaining degree budget with random forward edges
    for (int i = 0; i < n - 2; i++) {
        if (outDeg[i] <= 0) continue;

        std::vector<int> candidates;
        for (int j = i + 2; j < n; j++)
            candidates.push_back(j);
        std::shuffle(candidates.begin(), candidates.end(), rng);

        int added = 0;
        for (int j : candidates) {
            if (added >= outDeg[i]) break;
            int v = topo[j];
            if (!g.hasEdge(topo[i], v)) {
                g.addEdge(topo[i], v);
                added++;
            }
        }
    }

    return g;
}

// ─────────────────────────────────────────────────────────────────────────────
//  Public entry point
//
//  OLD: Graph generateTree(int n, bool directed)
//  NEW: renamed to generateDAG() — the result is a DAG, not a tree.
//  A tree has exactly n-1 edges; our directed graph has up to n*(n-1)/2 edges.
// ─────────────────────────────────────────────────────────────────────────────
Graph generateDAG(int n, bool directed) {
    if (directed) return buildDirectedDAG(n, EXTRA_EDGE_SCALE_DIRECTED);
    else          return buildUndirectedGraph(n);
}
