#ifndef ABC_H
#define ABC_H

#include <vector>
using namespace std;

struct ABCParams {
    int colonySize = 90;
    int iterations = 0;
    int eliteCount = 3;
    int trialLimit = 18;
    int localSearchPasses = 4;
    int localSearchTrials = 12;
    int onlookerCount = 45;
    int relinkingPeriod = 12;
    double scoutRandomRate = 0.35;
    double guidedRefinementProb = 0.85;
    unsigned int seed = 0;
};

struct ABCResult {
    std::vector<int> bestSequence;
    int bestMakespan = 0;
};

ABCResult run_bee_colony_pfsp(const std::vector<std::vector<int>>& tiempos,
                              int n,
                              int m,
                              const ABCParams& params);

int compute_makespan_abc(const std::vector<int>& secuencia,
                         const std::vector<std::vector<int>>& tiempos,
                         int m);

#endif
