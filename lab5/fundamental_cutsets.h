#pragma once
#include "boruvka.h"
#include "graph.h"
#include <vector>

struct CutsetEdge {
    int u, v;
};

struct FundamentalCutset {
    int index;
    MSTEdge treeEdge;
    std::vector<CutsetEdge> edges;
};

std::vector<FundamentalCutset> buildFundamentalCutsets(
        const Graph& g,
        const std::vector<MSTEdge>& mstEdges);

void printFundamentalCutsets(const std::vector<FundamentalCutset>& cutsets);

void printCutsetSymmetricDifference(
        const std::vector<FundamentalCutset>& cutsets,
        const std::vector<int>& selectedIndices);
