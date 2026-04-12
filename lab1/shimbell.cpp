#include "shimbell.h"
#include "distribution.h"
#include <cmath>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <sstream>

// ── Weight generation ─────────────────────────────────────────────────────────

static double sampleWeight(WeightMode mode) {
    // Scale: multiply N(0,1) by 5 and round, keep away from zero.
    double raw = normalSample(12) * 5.0;
    int w = static_cast<int>(std::round(raw));

    switch (mode) {
        case POSITIVE:
            return (w <= 0) ? 1 : w;   // guarantee > 0
        case NEGATIVE:
            return (w >= 0) ? -1 : w;  // guarantee < 0
        case MIXED:
        default:
            return (w == 0) ? 1 : w;   // just avoid zero
    }
}

Matrix generateWeightMatrix(const Graph& g, WeightMode mode) {
    int n = g.n;
    // Non-edges INF depending on min/max context.
    // We store SHIMBELL_INF; when used for max we negate on the fly inside shimbell().
    Matrix W(n, std::vector<double>(n, SHIMBELL_INF));

    for (int i = 0; i < n; i++) {
        W[i][i] = SHIMBELL_INF;   // no self-loop — exact k-step semantics
        for (int j = 0; j < n; j++) {
            if (i != j && g.hasEdge(i, j))
                W[i][j] = sampleWeight(mode);
        }
    }
    return W;
}

// ── Tropical matrix multiply ──────────────────────────────────────────────────

static Matrix tropicalMin(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());
    Matrix C(n, std::vector<double>(n, SHIMBELL_INF));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            if (A[i][k] >= SHIMBELL_INF) continue;
            for (int j = 0; j < n; j++) {
                if (B[k][j] >= SHIMBELL_INF) continue;
                double v = A[i][k] + B[k][j];
                if (v < C[i][j]) C[i][j] = v;
            }
        }
    return C;
}

static Matrix tropicalMax(const Matrix& A, const Matrix& B) {
    int n = static_cast<int>(A.size());
    Matrix C(n, std::vector<double>(n, SHIMBELL_NINF));
    for (int i = 0; i < n; i++)
        for (int k = 0; k < n; k++) {
            if (A[i][k] <= SHIMBELL_NINF) continue;
            for (int j = 0; j < n; j++) {
                if (B[k][j] <= SHIMBELL_NINF) continue;
                double v = A[i][k] + B[k][j];
                if (v > C[i][j]) C[i][j] = v;
            }
        }
    return C;
}

// ── Identity matrices ─────────────────────────────────────────────────────────

static Matrix identityMin(int n) {
    Matrix I(n, std::vector<double>(n, SHIMBELL_INF));
    for (int i = 0; i < n; i++) I[i][i] = 0.0;
    return I;
}

static Matrix identityMax(int n) {
    Matrix I(n, std::vector<double>(n, SHIMBELL_NINF));
    for (int i = 0; i < n; i++) I[i][i] = 0.0;
    return I;
}

// Build the "max" version of the weight matrix:
// edges keep their weight, non-edges → SHIMBELL_NINF, diagonal → SHIMBELL_NINF.
// W[i][i] == SHIMBELL_INF, so the condition below correctly skips the diagonal.
static Matrix toMaxMatrix(const Matrix& W) {
    int n = static_cast<int>(W.size());
    Matrix M(n, std::vector<double>(n, SHIMBELL_NINF));
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++) {
            if (W[i][j] < SHIMBELL_INF)   // real edges only (diagonal is INF -> skipped)
                M[i][j] = W[i][j];
        }
    return M;
}

// ── Public shimbell() ─────────────────────────────────────────────────────────

std::pair<Matrix, Matrix> shimbell(const Matrix& W, int k) {
    int n = static_cast<int>(W.size());

    if (k == 0)
        return { identityMin(n), identityMax(n) };

    Matrix Wmax = toMaxMatrix(W);

    Matrix minResult = W;
    Matrix maxResult = Wmax;

    for (int step = 1; step < k; step++) {
        minResult = tropicalMin(minResult, W);
        maxResult = tropicalMax(maxResult, Wmax);
    }

    return { minResult, maxResult };
}

// ── Print ─────────────────────────────────────────────────────────────────────

void printMatrix(const Matrix& M) {
    int n = static_cast<int>(M.size());
    const int W = 8;

    std::cout << "    ";
    for (int j = 0; j < n; j++)
        std::cout << std::setw(W) << j;
    std::cout << "\n    ";
    for (int j = 0; j < n; j++)
        std::cout << std::string(W, '-');
    std::cout << "\n";

    for (int i = 0; i < n; i++) {
        std::cout << std::setw(2) << i << " |";
        for (int j = 0; j < n; j++) {
            if (M[i][j] >= SHIMBELL_INF / 2)
                std::cout << std::setw(W) << "INF";
            else if (M[i][j] <= SHIMBELL_NINF / 2)
                std::cout << std::setw(W) << "-INF";
            else {
                std::cout << std::setw(W) << static_cast<long long>(M[i][j]);
            }
        }
        std::cout << "\n";
    }
}

/*
todo solved:
i. need to increse the parametter so that i can get more edges in the matrix
ii. in the shimbell : problem -> 0 also counting but it should not be
*/