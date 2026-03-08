#ifndef ACO_H
#define ACO_H

#include <vector>

struct ACOParams {
    int antCount = 25;
    int iterations = 0;
    double alpha = 1.0;
    double beta = 2.0;
    double rho = 0.15;
    double q0 = 0.85;
    double Q = 100.0;
    unsigned int seed = 0;
};

struct ACOResult {
    std::vector<int> bestSequence;
    int bestMakespan = 0;
};

ACOResult run_aco_pfsp(const std::vector<std::vector<int>>& tiempos,
                       int n,
                       int m,
                       const ACOParams& params);

int compute_makespan_aco(const std::vector<int>& secuencia,
                         const std::vector<std::vector<int>>& tiempos,
                         int m);

#endif
