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
// separate scales
static constexpr double EXTRA_EDGE_SCALE_DIRECTED   = 8.0;  // dense DAG
static constexpr double EXTRA_EDGE_SCALE_UNDIRECTED = 1.5;  // sparse enough to vary after symmetrize

static Graph buildDirectedDAG(int n, double scale);

//  Build an undirected graph from the same directed DAG model by ignoring
//  edge directions. This matches the lab wording: the undirected graph is
//  obtained from the generated oriented graph. Because the directed graph may
//  contain extra forward edges, the undirected version is connected and may
//  contain cycles, so MST algorithms are non-trivial.
// ─────────────────────────────────────────────────────────────────────────────
static Graph buildUndirectedGraph(int n) {
    Graph dag = buildDirectedDAG(n, EXTRA_EDGE_SCALE_UNDIRECTED);
    Graph undirected(n, false);

    for (int u = 0; u < n; u++)
        for (int v = 0; v < n; v++)
            if (dag.hasEdge(u, v))
                undirected.addEdge(u, v);

    return undirected;
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
static Graph buildDirectedDAG(int n, double scale) {
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
//  renamed to generateDAG() — the result is a DAG, not a tree.
//  A tree has exactly n-1 edges; our directed graph has up to n*(n-1)/2 edges.
// ─────────────────────────────────────────────────────────────────────────────
Graph generateDAG(int n, bool directed) {
    if (directed) return buildDirectedDAG(n, EXTRA_EDGE_SCALE_DIRECTED);
    else          return buildUndirectedGraph(n);
}
