/*
  Graph Theory (Lab 1)
  Variant: 36
  Distribution: Normal (Gaussian) - Vadzinsky reference book

  Box-Muller (Vadzinsky p.106):
    x = sqrt(-2 * ln(r1)) * cos(2 * pi * r2)
    y = mu + sigma * x

  GENERATION BY VERTEX DEGREES:
    Generate degree sequence via Normal distribution,
    adjust sum to 2*(n-1) for undirected tree (n-1 edges),
    build Prufer sequence from degrees, decode to tree.

  GRAPH MODES:
    - Undirected tree: sum degrees = 2*(n-1)
    - Directed graph:  generated from undirected by assigning directions
      (symmetric adjacency matrix -> asymmetric for directed)
 */

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <climits>
#include <queue>
#include <algorithm>
#include <iomanip>
#include <string>
#include <numeric>
#include <functional>

using namespace std;


//  CONSTANTS
const double PI_VAL = 3.14159265358979323846;
const double INF_D  = 1e15;
const int    INF_I  = INT_MAX / 2;


//  NORMAL DISTRIBUTION (Box-Muller, Vadzinsky p.106)
double normalRandom(double mu = 0.0, double sigma = 1.0) {
    double r1, r2;
    do { r1 = (double)rand() / RAND_MAX; } while (r1 == 0.0);
    r2 = (double)rand() / RAND_MAX;
    double x = sqrt(-2.0 * log(r1)) * cos(2.0 * PI_VAL * r2);
    return mu + sigma * x;
}


//  GLOBAL STATE
int n = 0;

// Undirected adjacency (tree base)
vector<vector<int>>    adjUndirected;   // adjacency list  (undirected)
vector<vector<int>>    matUndirected;   // adjacency matrix (undirected, 0/1)

// Directed adjacency (derived from undirected)
vector<vector<int>>    adjDirected;     // adjacency list  (directed)
vector<vector<int>>    matDirected;     // adjacency matrix (directed, 0/1)

// Weight matrix (for Shimbel)
vector<vector<double>> weightMatrix;

//  HELPERS
void line(int len = 60) {
    cout << "  ";
    for (int i = 0; i < len; i++) cout << "-";
    cout << "\n";
}
void header(const string& t) { cout << "\n"; line(); cout << "  " << t << "\n"; line(); }

//  TASK 1A: Generate undirected tree (connected acyclic)
//  Steps:
//    1. Generate degrees with Normal distribution
//    2. Build Prufer sequence from degrees
//    3. Decode Prufer -> edges
void generateTree() {
    adjUndirected.assign(n, vector<int>());
    matUndirected.assign(n, vector<int>(n, 0));
    adjDirected.assign(n, vector<int>());
    matDirected.assign(n, vector<int>(n, 0));

    if (n == 1) {
        cout << "\n  Single vertex - no edges.\n"; return;
    }
    if (n == 2) {
        adjUndirected[0].push_back(1); adjUndirected[1].push_back(0);
        matUndirected[0][1] = matUndirected[1][0] = 1;
        // Directed: randomly assign direction for 1 edge
        if (rand() % 2) { adjDirected[0].push_back(1); matDirected[0][1] = 1; }
        else             { adjDirected[1].push_back(0); matDirected[1][0] = 1; }
        cout << "\n  Tree with 2 vertices generated.\n"; return;
    }

    //Step 1: Normal distribution degrees
    double mu_deg = 2.0, sigma_deg = 1.0;
    vector<int> deg(n);
    for (int i = 0; i < n; i++)
        deg[i] = max(1, (int)round(fabs(normalRandom(mu_deg, sigma_deg))));

    // Step 2: Adjust sum to 2*(n-1)
    int target = 2 * (n - 1);
    int cur = 0; for (int d : deg) cur += d;
    while (cur != target) {
        int idx = rand() % n;
        if (cur < target)                    { deg[idx]++; cur++; }
        else if (cur > target && deg[idx]>1) { deg[idx]--; cur--; }
    }

    cout << "\n  Degrees generated (Normal, mu=2, sigma=1):\n  ";
    for (int i = 0; i < n; i++) cout << " v" << i+1 << "=" << deg[i];


    // Step 3: Build Prufer sequence
    // vertex i appears (deg[i]-1) times
    vector<int> prufer;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < deg[i]-1; j++)
            prufer.push_back(i);
    // shuffle for randomness
    for (int i = (int)prufer.size()-1; i > 0; i--) {
        int j = rand() % (i+1); swap(prufer[i], prufer[j]);
    }

    // Step 4: Decode Prufer -> tree edges
    vector<int> degree(n, 1);
    for (int v : prufer) degree[v]++;

    for (int v : prufer) {
        for (int u = 0; u < n; u++) {
            if (degree[u] == 1) {
                adjUndirected[v].push_back(u);
                adjUndirected[u].push_back(v);
                matUndirected[v][u] = matUndirected[u][v] = 1;
                degree[v]--; degree[u]--;
                break;
            }
        }
    }
    vector<int> last;
    for (int i = 0; i < n; i++) if (degree[i]==1) last.push_back(i);
    if ((int)last.size()==2) {
        adjUndirected[last[0]].push_back(last[1]);
        adjUndirected[last[1]].push_back(last[0]);
        matUndirected[last[0]][last[1]] = matUndirected[last[1]][last[0]] = 1;
    }
    for (int i = 0; i < n; i++) sort(adjUndirected[i].begin(), adjUndirected[i].end());

    // Build directed version: randomly orient each undirected edge
    for (int i = 0; i < n; i++)
        for (int j : adjUndirected[i])
            if (i < j) {
                if (rand() % 2) {
                    adjDirected[i].push_back(j);
                    matDirected[i][j] = 1;
                } else {
                    adjDirected[j].push_back(i);
                    matDirected[j][i] = 1;
                }
            }
    for (int i = 0; i < n; i++) sort(adjDirected[i].begin(), adjDirected[i].end());

    cout << "\n Vertices=" << n << "  Edges=" << n-1 << "\n";
    cout << "  Use option 2 to view connections.\n";
}


//  PRINT ADJACENCY LIST + MATRIX  (undirected OR directed)
void printGraph(bool directed) {
    auto& adj = directed ? adjDirected : adjUndirected;
    auto& mat = directed ? matDirected : matUndirected;
    string mode = directed ? "DIRECTED" : "UNDIRECTED";

    header("ADJACENCY LIST  [" + mode + "]");
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "  Vertex " << setw(3) << i+1 << "  -->  ";
        if (adj[i].empty()) cout << "(none)";
        else for (int k = 0; k < (int)adj[i].size(); k++) {
            if (k) cout << ", ";
            cout << adj[i][k]+1;
        }
        cout << "   (degree=" << adj[i].size() << ")\n";
    }

    cout << "\n  EDGE LIST:\n";
    for (int i = 0; i < n; i++)
        for (int j : adj[i])
            if (!directed ? i<j : true)
                cout << "    " << i+1 << (directed?" --> ":" ---- ") << j+1 << "\n";

    header("ADJACENCY MATRIX  [" + mode + "]");
    cout << "\n       ";
    for (int j = 0; j < n; j++) cout << setw(4) << j+1;
    cout << "\n  ";
    for (int j = 0; j < 5+4*n; j++) cout << "-";
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << setw(3) << i+1 << " |";
        for (int j = 0; j < n; j++) cout << setw(4) << mat[i][j];
        cout << "\n";
    }
    if (!directed)
        cout << "\n  Note: Undirected => matrix is symmetric (mat[i][j] == mat[j][i])\n";
    else
        cout << "\n  Note: Directed => matrix may NOT be symmetric\n";
}

// ============================================================
//  BFS distances  (for undirected OR directed graph)
//  Returns vector dist[i] = shortest hop-distance from src to i
//  dist[i] = -1 if unreachable
// ============================================================
vector<int> bfsDistances(int src, bool directed) {
    auto& adj = directed ? adjDirected : adjUndirected;
    vector<int> dist(n, -1);
    queue<int> q;
    dist[src] = 0; q.push(src);
    while (!q.empty()) {
        int u = q.front(); q.pop();
        for (int v : adj[u])
            if (dist[v]==-1) { dist[v]=dist[u]+1; q.push(v); }
    }
    return dist;
}


//  TASK 2: Eccentricities, Center, Diametral vertices
//  Works for BOTH undirected and directed (user chooses)
//
//  For directed graph: distance(u,v) may differ from distance(v,u)
//  Eccentricity(v) = max distance from v to any REACHABLE vertex
void computeEccentricities() {
    header("TASK 2: ECCENTRICITIES, CENTER, DIAMETER");

    cout << "\n  Compute for which graph?\n";
    cout << "    1 - Undirected\n";
    cout << "    2 - Directed\n";
    cout << "  Your choice: ";
    int gtype; cin >> gtype;
    bool directed = (gtype == 2);
    string mode = directed ? "DIRECTED" : "UNDIRECTED";
    cout << "\n  [MODE: " << mode << "]\n";

    // Build full distance matrix
    vector<vector<int>> distMat(n);
    for (int i = 0; i < n; i++)
        distMat[i] = bfsDistances(i, directed);

    // Print distance matrix
    cout << "\n  DISTANCE MATRIX:\n\n       ";
    for (int j = 0; j < n; j++) cout << setw(5) << j+1;
    cout << "\n  ";
    for (int j = 0; j < 6+5*n; j++) cout << "-";
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << setw(3) << i+1 << " |";
        for (int j = 0; j < n; j++) {
            if (distMat[i][j]==-1) cout << setw(5) << "inf";
            else                   cout << setw(5) << distMat[i][j];
        }
        cout << "\n";
    }

    // Eccentricities
    vector<int> ecc(n, 0);
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i!=j && distMat[i][j]!=-1)
                ecc[i] = max(ecc[i], distMat[i][j]);

    int radius   = *min_element(ecc.begin(), ecc.end());
    int diameter = *max_element(ecc.begin(), ecc.end());

    cout << "\n  Vertex  | Eccentricity | Role\n";
    cout << "  --------+--------------+--------------------------\n";
    for (int i = 0; i < n; i++) {
        cout << "    " << setw(4) << i+1 << "  |      " << setw(4) << ecc[i] << "    | ";
        if      (ecc[i]==radius && ecc[i]==diameter) cout << "CENTER + DIAMETRICAL";
        else if (ecc[i]==radius)                     cout << "CENTER";
        else if (ecc[i]==diameter)                   cout << "DIAMETRICAL";
        else cout << "-";
        cout << "\n";
    }

    cout << "\n  Radius   = " << radius   << "\n";
    cout << "  Diameter = " << diameter << "\n";

    // Centers stored in {}
    cout << "\n  CENTER vertices      = { ";
    for (int i = 0; i < n; i++) if (ecc[i]==radius)   cout << i+1 << " ";
    cout << "}\n";

    // Diametrical stored in {}
    cout << "  DIAMETRICAL vertices = { ";
    for (int i = 0; i < n; i++) if (ecc[i]==diameter) cout << i+1 << " ";
    cout << "}\n";
}


//  TASK 3: Shimbel Method
//
//  Shimbel's algorithm:
//    D^(k)[i][j] = shortest path from i to j using at most k edges
//    D^(1) = weight matrix (direct edges)
//    D^(k)[i][j] = min over all m of ( D^(k-1)[i][m] + W[m][j] )
//
//  We iterate for k = 1 .. n-1, updating the matrix each step.
//  This correctly finds shortest paths of EXACTLY k edges,
//  and the running minimum gives shortest path of AT MOST k edges.
//
//  When edges=0 user input: show identity matrix (zeros on diagonal,
//  dash everywhere else) as described in the requirement.

void generateWeightMatrix(int mode) {
    double wMu=5.0, wSigma=2.0;
    weightMatrix.assign(n, vector<double>(n, INF_D));
    for (int i = 0; i < n; i++) weightMatrix[i][i] = 0;

    for (int i = 0; i < n; i++)
        for (int j : adjUndirected[i])
            if (i < j) {
                double w = fabs(normalRandom(wMu, wSigma)) + 0.1;
                if      (mode==1) w =  fabs(w);
                else if (mode==2) w = -fabs(w);
                else              w = (rand()%2) ? fabs(w) : -fabs(w);
                weightMatrix[i][j] = w;
                weightMatrix[j][i] = w;
            }
}

void printWeightMatrix() {
    cout << "\n  WEIGHT MATRIX (Normal distribution, INF = no direct edge):\n\n       ";
    for (int i = 0; i < n; i++) cout << setw(9) << i+1;
    cout << "\n  ";
    for (int i = 0; i < 7+9*n; i++) cout << "-";
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << setw(3) << i+1 << " | ";
        for (int j = 0; j < n; j++) {
            if      (weightMatrix[i][j] >=  INF_D/2) cout << setw(9) << "INF";
            else if (weightMatrix[i][j] <= -INF_D/2) cout << setw(9) << "-INF";
            else cout << setw(9) << fixed << setprecision(2) << weightMatrix[i][j];
        }
        cout << "\n";
    }
}

// Print the "0 edges" identity-like matrix (requirement: show dash for non-diagonal)
void printZeroEdgeMatrix() {
    cout << "\n  Matrix for 0 edges (only trivial self-paths):\n\n       ";
    for (int i = 0; i < n; i++) cout << setw(5) << i+1;
    cout << "\n  ";
    for (int i = 0; i < 5+5*n; i++) cout << "-";
    cout << "\n";
    for (int i = 0; i < n; i++) {
        cout << "  " << setw(3) << i+1 << " | ";
        for (int j = 0; j < n; j++)
            cout << setw(5) << (i==j ? "0" : "-");
        cout << "\n";
    }
}

void shimbelMethod() {
    header("TASK 3: SHIMBEL METHOD");

    int edgesInput;
    cout << "\n  Enter number of edges (tree has " << n-1 << " edges): ";
    cin >> edgesInput;

    // Special case: 0 edges -> identity matrix
    if (edgesInput == 0) {
        printZeroEdgeMatrix();
        return;
    }

    int modeW;
    cout << "\n  Weight generation mode:\n";
    cout << "    1 - Positive only\n";
    cout << "    2 - Negative only\n";
    cout << "    3 - Mixed\n";
    cout << "  Your choice: ";
    cin >> modeW;
    if (modeW<1||modeW>3) { cout << "  [ERROR] Enter 1-3.\n"; return; }

    generateWeightMatrix(modeW);
    printWeightMatrix();

    int choice;
    cout << "\n  Show:\n";
    cout << "    1 - Minimum path matrix\n";
    cout << "    2 - Maximum path matrix\n";
    cout << "    3 - Both\n";
    cout << "  Your choice: ";
    cin >> choice;

    // SHIMBEL: MINIMUM PATHS
    // D[i][j] = current best (shortest) distance i->j
    // Iterate: for each intermediate vertex k,
    //   D[i][j] = min(D[i][j], D[i][k] + W[k][j])
    vector<vector<double>> minP(n, vector<double>(n, INF_D));
    for (int i = 0; i < n; i++) minP[i][i] = 0;
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (weightMatrix[i][j] < INF_D/2)
                minP[i][j] = weightMatrix[i][j];

    // Shimbel iteration (equivalent to Floyd-Warshall for min)
    for (int k = 0; k < n; k++)           // intermediate vertex k
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (minP[i][k]<INF_D/2 && minP[k][j]<INF_D/2)
                    minP[i][j] = min(minP[i][j], minP[i][k]+minP[k][j]);

    // SHIMBEL: MAXIMUM PATHS (DFS backtracking on tree)
    // In a tree there are no cycles -> max path = path with most weight edges
    vector<vector<double>> maxP(n, vector<double>(n, -INF_D));
    for (int i = 0; i < n; i++) maxP[i][i] = 0;

    // DFS to find maximum weight simple path between every pair
    function<void(int,int,int,double,vector<bool>&)> dfsMax =
        [&](int start, int cur, int target, double curW, vector<bool>& vis) {
            if (cur==target) { maxP[start][target]=max(maxP[start][target],curW); return; }
            vis[cur]=true;
            for (int v : adjUndirected[cur])
                if (!vis[v] && weightMatrix[cur][v]<INF_D/2)
                    dfsMax(start, v, target, curW+weightMatrix[cur][v], vis);
            vis[cur]=false;
        };

    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            if (i!=j) {
                vector<bool> vis(n,false);
                dfsMax(i, i, j, 0.0, vis);
            }

    // Print function
    auto printMat = [&](const string& title, const vector<vector<double>>& M) {
        cout << "\n  " << title << "\n\n       ";
        for (int i = 0; i < n; i++) cout << setw(9) << i+1;
        cout << "\n  ";
        for (int i = 0; i < 7+9*n; i++) cout << "-";
        cout << "\n";
        for (int i = 0; i < n; i++) {
            cout << "  " << setw(3) << i+1 << " | ";
            for (int j = 0; j < n; j++) {
                if      (M[i][j]>= INF_D/2) cout << setw(9) << "INF";
                else if (M[i][j]<=-INF_D/2) cout << setw(9) << "-INF";
                else cout << setw(9) << fixed << setprecision(2) << M[i][j];
            }
            cout << "\n";
        }
    };

    if (choice==1||choice==3) printMat("MINIMUM PATH MATRIX (Shimbel):", minP);
    if (choice==2||choice==3) printMat("MAXIMUM PATH MATRIX (Shimbel):", maxP);

    cout << "\n  READ: matrix[i][j] = total weight of best path from vertex i to vertex j\n";
    cout << "  Diagonal = 0 (no cost to stay at same vertex)\n";
}


//  TASK 4: Route existence + count
//  DFS with backtracking finds ALL simple paths
vector<vector<int>> allRoutes;

void dfsFindRoutes(int cur, int dest, bool directed,
                   vector<bool>& vis, vector<int>& path) {
    auto& adj = directed ? adjDirected : adjUndirected;
    vis[cur]=true; path.push_back(cur);
    if (cur==dest) { allRoutes.push_back(path); }
    else for (int v : adj[cur]) if (!vis[v]) dfsFindRoutes(v,dest,directed,vis,path);
    path.pop_back(); vis[cur]=false;
}

void findRoutes() {
    header("TASK 4: ROUTE EXISTENCE AND COUNT");

    cout << "\n  Graph type:\n    1 - Undirected\n    2 - Directed\n  Your choice: ";
    int gtype; cin >> gtype;
    bool directed = (gtype==2);
    cout << "  [MODE: " << (directed?"DIRECTED":"UNDIRECTED") << "]\n";

    int src, dst;
    cout << "\n  Enter SOURCE vertex      (1 to " << n << "): "; cin >> src;
    cout << "  Enter DESTINATION vertex (1 to " << n << "): "; cin >> dst;

    if (src<1||src>n||dst<1||dst>n) {
        cout << "\n  [ERROR] Vertices must be between 1 and " << n << ".\n"; return;
    }
    if (src==dst) {
        cout << "\n  Source == Destination.\n  Route exists: YES\n";
        cout << "  Number of routes: 1   Path: { " << src << " }\n"; return;
    }

    src--; dst--;
    allRoutes.clear();
    vector<bool> vis(n,false);
    vector<int> path;
    dfsFindRoutes(src, dst, directed, vis, path);

    cout << "\n  From vertex " << src+1 << " to vertex " << dst+1 << ":\n";
    cout << "  Route exists    : " << (allRoutes.empty()?"NO":"YES") << "\n";
    cout << "  Number of routes: " << allRoutes.size() << "\n";

    if (!allRoutes.empty()) {
        cout << "\n  Routes:\n";
        for (int i = 0; i < (int)allRoutes.size(); i++) {
            cout << "    Route " << i+1 << ":  { ";
            for (int k = 0; k < (int)allRoutes[i].size(); k++) {
                if (k) cout << " -> ";
                cout << allRoutes[i][k]+1;
            }
            cout << " }\n";
        }
        if ((int)allRoutes.size()==1 && !directed)
            cout << "\n  (In an undirected tree there is exactly 1 simple path between any two vertices)\n";
    }
}





//  MENU
void printMenu() {
    cout << "\n";
    cout << "  +==============================================================+\n";
    cout << "  |  Graph Theory (Lab 1)                        Variant 36     |\n";
    cout << "  |  Distribution: Normal (Vadzinsky p.106)                     |\n";
    cout << "  +==============================================================+\n";
    cout << "  |  1. Generate tree (degrees via Normal distribution)         |\n";
    cout << "  |  2. Print adjacency list + matrix (undirected or directed)  |\n";
    cout << "  |  3. Eccentricities, center, diametral vertices              |\n";
    cout << "  |  4. Shimbel method (min/max path matrices)                  |\n";
    cout << "  |  5. Route existence and count                               |\n";
    cout << "  |  0. Exit                                                    |\n";
    cout << "  +==============================================================+\n";
    cout << "  Your choice: ";
}


//  MAIN
int main() {
    srand((unsigned)time(nullptr));

    cout << "\n";
    cout << "  ============================================================\n";
    cout << "    Graph Theory (Lab 1) | Variant 36 | Normal Distribution\n";
    cout << "    Normal: x = sqrt(-2*ln(r1)) * cos(2*pi*r2)\n";
    cout << "    Then: y = mu + sigma * x\n";
    cout << "  ============================================================\n";

    int choice;
    do {
        printMenu();
        cin >> choice;
        switch (choice) {
            case 1: {
                cout << "\n  Enter number of vertices : ";
                cin >> n;
                if (n<1||n>30) { cout << "  [ERROR] 1 to 30.\n"; n=0; }
                else generateTree();
                break;
            }
            case 2: {
                if (!n) { cout << "\n  [!] Generate tree first (option 1).\n"; break; }
                cout << "\n  Show:\n    1 - Undirected\n    2 - Directed\n  Choice: ";
                int g; cin >> g; printGraph(g==2); break;
            }
            case 3: if (!n) { cout << "\n  [!] Generate tree first.\n"; break; }
                    computeEccentricities(); break;
            case 4: if (!n) { cout << "\n  [!] Generate tree first.\n"; break; }
                    shimbelMethod(); break;
            case 5: if (!n) { cout << "\n  [!] Generate tree first.\n"; break; }
                    findRoutes(); break;
            case 0: cout << "\n  Goodbye!\n"; break;
            default: cout << "\n  [!] Enter 0-5.\n";
        }
    } while (choice != 0);
    return 0;
}