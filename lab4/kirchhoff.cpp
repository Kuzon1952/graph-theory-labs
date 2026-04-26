#include "kirchhoff.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// Gaussian elimination over doubles; returns determinant.
// Uses partial pivoting for numerical stability.
static double gaussianDet(std::vector<std::vector<double>> M) {
    int n = (int)M.size();
    double det = 1.0;
    for (int col = 0; col < n; col++) {
        // Find pivot
        int pivot = -1;
        double best = 0.0;
        for (int row = col; row < n; row++) {
            if (std::abs(M[row][col]) > best) {
                best = std::abs(M[row][col]);
                pivot = row;
            }
        }
        if (pivot == -1 || best < 1e-9) return 0.0;
        if (pivot != col) {
            std::swap(M[pivot], M[col]);
            det = -det;
        }
        det *= M[col][col];
        double inv = 1.0 / M[col][col];
        for (int row = col + 1; row < n; row++) {
            double factor = M[row][col] * inv;
            for (int k = col; k < n; k++)
                M[row][k] -= factor * M[col][k];
        }
    }
    return det;
}

long long countSpanningTrees(const Graph& g) {
    int n = g.n;
    if (n < 2) return 0;

    // Build Laplacian treating graph as undirected
    std::vector<std::vector<double>> L(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            // Edge exists in either direction for undirected treatment
            bool edge = g.hasEdge(i, j) || g.hasEdge(j, i);
            if (edge) {
                L[i][j] -= 1.0;
                L[i][i] += 1.0;
            }
        }
        // Remove double-counting for undirected: each undirected edge (i,j)
        // should only count once. Since we check both (i,j) and (j,i),
        // for undirected graph hasEdge already returns true for both.
        // For directed graph, we're symmetrising here.
    }
    // Correct: for undirected, L[i][i] = degree(i) counted correctly above
    // because we only increment when the edge exists (and for undirected
    // hasEdge(i,j)==hasEdge(j,i), so we'd double-count — fix below).
    if (!g.directed) {
        // For undirected, the loop above counts each edge twice for L[i][i].
        // Reset and recount properly.
        for (int i = 0; i < n; i++) {
            L[i][i] = 0.0;
            for (int j = 0; j < n; j++)
                if (i != j && g.hasEdge(i, j))
                    L[i][i] += 1.0;
            // Off-diagonal already set correctly above
        }
    }

    // Remove row 0 and col 0 → (n-1)×(n-1) cofactor matrix
    std::vector<std::vector<double>> sub(n - 1, std::vector<double>(n - 1));
    for (int i = 1; i < n; i++)
        for (int j = 1; j < n; j++)
            sub[i - 1][j - 1] = L[i][j];

    double d = gaussianDet(sub);
    return static_cast<long long>(std::round(d));
}

void printKirchhoffResult(const Graph& g, long long count) {
    int n = g.n;
    const std::string sep(60, '-');

    std::cout << "\n  " << sep << "\n";
    std::cout << "  KIRCHHOFF'S MATRIX-TREE THEOREM\n";
    std::cout << "  " << sep << "\n";

    // Print Laplacian
    std::vector<std::vector<double>> L(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            bool edge = g.hasEdge(i, j) || g.hasEdge(j, i);
            if (edge) { L[i][j] = -1.0; L[i][i] += 1.0; }
        }
    }
    if (!g.directed) {
        for (int i = 0; i < n; i++) {
            L[i][i] = 0.0;
            for (int j = 0; j < n; j++)
                if (i != j && g.hasEdge(i, j)) L[i][i] += 1.0;
        }
    }

    const int CW = 5;
    std::cout << "\n  Kirchhoff (Laplacian) matrix B:\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(CW) << (int)L[i][j];
        std::cout << "\n";
    }

    std::cout << "\n  Cofactor A_11 (remove row 1, col 1):\n";
    std::cout << "    det of " << (n - 1) << "x" << (n - 1) << " submatrix = "
              << count << "\n";
    std::cout << "\n  Number of spanning trees = " << count << "\n";
    std::cout << "\n  " << sep << "\n";
}
