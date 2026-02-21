#ifndef GA_H
#define GA_H

#include <vector>

struct GAParams {
    int populationSize = 60;
    double crossoverProb = 0.80;
    double mutationProb = 0.15;
    int iterations = 0;
    unsigned int seed = 0;
};

struct GAResult {
    std::vector<int> bestSequence;
    int bestMakespan = 0;
};

GAResult run_genetic_algorithm(const std::vector<std::vector<int>>& tiempos,
                               int n,
                               int m,
                               const GAParams& params);

#endif
