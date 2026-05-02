#include "kirchhoff.h"
#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>

// Build the Kirchhoff (Laplacian) matrix treating the graph as undirected.
// L[i][j] = -1 if edge {i,j} exists (in either direction), L[i][i] = degree(i).
static std::vector<std::vector<double>> buildLaplacian(const Graph& g) {
    const int n = g.n;
    std::vector<std::vector<double>> L(n, std::vector<double>(n, 0.0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (i == j) continue;
            if (g.hasEdge(i, j) || g.hasEdge(j, i)) {
                L[i][j] = -1.0;
                L[i][i] += 1.0;
            }
        }
    }
    return L;
}

// Gaussian elimination determinant with partial pivoting.
static double gaussianDet(std::vector<std::vector<double>> M) {
    int n = (int)M.size();
    double det = 1.0;
    for (int col = 0; col < n; col++) {
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

    auto L = buildLaplacian(g);

    // Cofactor A_{00}: remove row 0 and col 0, compute det of (n-1)x(n-1) submatrix
    std::vector<std::vector<double>> sub(n - 1, std::vector<double>(n - 1));
    for (int i = 1; i < n; i++)
        for (int j = 1; j < n; j++)
            sub[i - 1][j - 1] = L[i][j];

    double d = gaussianDet(sub);
    return static_cast<long long>(std::round(d));
}

void printKirchhoffResult(const Graph& g, long long count) {
    const int n = g.n;
    const std::string sep(60, '-');

    std::cout << "\n  " << sep << "\n";
    std::cout << "  KIRCHHOFF'S MATRIX-TREE THEOREM\n";
    std::cout << "  " << sep << "\n";

    auto L = buildLaplacian(g);

    const int CW = 5;
    std::cout << "\n  Kirchhoff (Laplacian) matrix B:\n";
    std::cout << "  B[i][i] = degree(i),  B[i][j] = -1 if edge {i,j}, else 0\n\n";
    std::cout << "       ";
    for (int j = 0; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + n * CW, '-') << "\n";
    for (int i = 0; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 0; j < n; j++)
            std::cout << std::setw(CW) << (int)L[i][j];
        std::cout << "\n";
    }

    // Print cofactor submatrix (remove row 1, col 1)
    std::cout << "\n  Cofactor A_11 — delete row 1 and col 1:\n\n";
    std::cout << "       ";
    for (int j = 1; j < n; j++) std::cout << std::setw(CW) << j + 1;
    std::cout << "\n  " << std::string(7 + (n - 1) * CW, '-') << "\n";
    for (int i = 1; i < n; i++) {
        std::cout << "  " << std::setw(3) << i + 1 << " |";
        for (int j = 1; j < n; j++)
            std::cout << std::setw(CW) << (int)L[i][j];
        std::cout << "\n";
    }

    std::cout << "\n  det(A_11) = " << count << "\n";
    std::cout << "  Number of spanning trees = " << count << "\n";
    std::cout << "\n  " << sep << "\n";
}
