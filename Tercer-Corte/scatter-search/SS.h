#ifndef MA_H
#define MA_H

#include <vector>
using namespace std;
struct SSParams {
    int populationSize = 80;
    int iterations = 0;
    int tournamentSize = 3;
    double recombinationProb = 0.90;
    double mutationProb = 0.12;
    int localSearchIters = 1;
    int eliteCount = 2;
    int diversityThreshold = 25;
    double pathRelinkingRate = 0.20;
    unsigned int seed = 0;
};

struct SSResult {
    std::vector<int> bestSequence;
    int bestMakespan = 0;
};

SSResult run_memetic_algorithm(const vector<vector<int>>& tiempos,
                               int n,
                               int m,
                               const SSParams& params);

int compute_makespan_ma(const std::vector<int>& secuencia,
                        const std::vector<std::vector<int>>& tiempos,
                        int m);

#endif
