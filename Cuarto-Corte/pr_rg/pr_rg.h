#ifndef PR_RG_H
#define PR_RG_H

#include <vector>
using namespace std;

struct PRParams {
    int networkSize = 90;
    int iterations = 0;
    int eliteCount = 3;
    int boredomLimit = 18;
    int localSearchPasses = 4;
    int localSearchTrials = 12;
    int listenerCount = 45;
    int relinkingPeriod = 12;
    double explorerRandomRate = 0.35;
    double guidedRefinementProb = 0.85;
    unsigned int seed = 0;
};

struct PRResult {
    std::vector<int> bestSequence;
    int bestMakespan = 0;
};

PRResult run_rumor_propagation_pfsp(const std::vector<std::vector<int>>& tiempos,
                                    int n,
                                    int m,
                                    const PRParams& params);

int compute_makespan_pr(const std::vector<int>& secuencia,
                        const std::vector<std::vector<int>>& tiempos,
                        int m);

#endif
