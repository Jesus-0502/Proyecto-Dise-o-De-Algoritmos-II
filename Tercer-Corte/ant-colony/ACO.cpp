#include "ACO.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

int compute_makespan_aco(const vector<int>& secuencia,
                         const vector<vector<int>>& tiempos,
                         int m) {
    int n = static_cast<int>(secuencia.size());
    if (n == 0) return 0;

    vector<vector<int>> C(n + 1, vector<int>(m + 1, 0));
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            C[i][j] = max(C[i - 1][j], C[i][j - 1]) + tiempos[secuencia[i - 1]][j - 1];
        }
    }
    return C[n][m];
}

static vector<int> neh_seed(const vector<vector<int>>& tiempos, int n, int m) {
    vector<pair<int, int>> sumTimes;
    sumTimes.reserve(n);
    for (int job = 0; job < n; ++job) {
        int s = accumulate(tiempos[job].begin(), tiempos[job].end(), 0);
        sumTimes.push_back({s, job});
    }
    sort(sumTimes.begin(), sumTimes.end(), greater<pair<int, int>>());

    vector<int> seq;
    for (int i = 0; i < n; ++i) {
        int job = sumTimes[i].second;
        int bestMs = numeric_limits<int>::max();
        vector<int> best;

        for (int p = 0; p <= static_cast<int>(seq.size()); ++p) {
            vector<int> cand = seq;
            cand.insert(cand.begin() + p, job);
            int ms = compute_makespan_aco(cand, tiempos, m);
            if (ms < bestMs) {
                bestMs = ms;
                best = cand;
            }
        }
        seq = best;
    }
    return seq;
}

static int sample_next_job(const vector<double>& weights, mt19937& rng) {
    double total = 0.0;
    for (double w : weights) total += w;

    if (total <= 0.0) {
        uniform_int_distribution<int> dist(0, static_cast<int>(weights.size()) - 1);
        return dist(rng);
    }

    uniform_real_distribution<double> dist(0.0, total);
    double r = dist(rng);
    double acc = 0.0;
    for (int i = 0; i < static_cast<int>(weights.size()); ++i) {
        acc += weights[i];
        if (r <= acc) return i;
    }
    return static_cast<int>(weights.size()) - 1;
}

static vector<int> construct_solution(const vector<vector<double>>& tau,
                                      const vector<double>& jobHeuristic,
                                      int n,
                                      int m,
                                      const ACOParams& params,
                                      mt19937& rng) {
    (void)m;
    vector<int> solution;
    solution.reserve(n);
    vector<char> used(n, false);
    uniform_real_distribution<double> prob(0.0, 1.0);

    for (int pos = 0; pos < n; ++pos) {
        vector<int> candidates;
        candidates.reserve(n - pos);
        vector<double> desirability;
        desirability.reserve(n - pos);

        for (int job = 0; job < n; ++job) {
            if (used[job]) continue;

            double eta = jobHeuristic[job];
            double val = pow(tau[pos][job], params.alpha) * pow(eta, params.beta);

            candidates.push_back(job);
            desirability.push_back(val);
        }

        int chosenIdx = 0;
        if (prob(rng) <= params.q0) {
            chosenIdx = static_cast<int>(max_element(desirability.begin(), desirability.end()) - desirability.begin());
        } else {
            chosenIdx = sample_next_job(desirability, rng);
        }

        int chosenJob = candidates[chosenIdx];
        solution.push_back(chosenJob);
        used[chosenJob] = true;
    }

    return solution;
}

ACOResult run_aco_pfsp(const vector<vector<int>>& tiempos,
                       int n,
                       int m,
                       const ACOParams& params) {
    mt19937 rng(params.seed == 0 ? random_device{}() : params.seed);

    vector<vector<double>> tau(n, vector<double>(n, 1.0));
    vector<double> jobHeuristic(n, 0.0);
    for (int job = 0; job < n; ++job) {
        int total = accumulate(tiempos[job].begin(), tiempos[job].end(), 0);
        jobHeuristic[job] = 1.0 / (1.0 + static_cast<double>(total));
    }

    vector<int> seedSeq = neh_seed(tiempos, n, m);
    int seedMs = compute_makespan_aco(seedSeq, tiempos, m);

    ACOResult best;
    best.bestSequence = seedSeq;
    best.bestMakespan = seedMs;

    const double tauMin = 1e-6;
    const double tauMax = 1e6;

    for (int iter = 0; iter < params.iterations; ++iter) {
        vector<vector<int>> antSolutions;
        vector<int> antMs;
        antSolutions.reserve(params.antCount);
        antMs.reserve(params.antCount);

        for (int k = 0; k < params.antCount; ++k) {
            vector<int> sol = construct_solution(tau, jobHeuristic, n, m, params, rng);
            int ms = compute_makespan_aco(sol, tiempos, m);

            antSolutions.push_back(sol);
            antMs.push_back(ms);

            if (ms < best.bestMakespan) {
                best.bestMakespan = ms;
                best.bestSequence = sol;
            }
        }

        for (int pos = 0; pos < n; ++pos) {
            for (int job = 0; job < n; ++job) {
                tau[pos][job] *= (1.0 - params.rho);
                if (tau[pos][job] < tauMin) tau[pos][job] = tauMin;
            }
        }

        vector<int> idx(params.antCount);
        iota(idx.begin(), idx.end(), 0);
        sort(idx.begin(), idx.end(), [&](int a, int b) {
            return antMs[a] < antMs[b];
        });

        int depositants = min(3, params.antCount);
        for (int rank = 0; rank < depositants; ++rank) {
            int ant = idx[rank];
            double delta = params.Q / static_cast<double>(antMs[ant]);
            for (int pos = 0; pos < n; ++pos) {
                int job = antSolutions[ant][pos];
                tau[pos][job] += delta;
                if (tau[pos][job] > tauMax) tau[pos][job] = tauMax;
            }
        }

        double deltaBest = params.Q / static_cast<double>(best.bestMakespan);
        for (int pos = 0; pos < n; ++pos) {
            int job = best.bestSequence[pos];
            tau[pos][job] += deltaBest;
            if (tau[pos][job] > tauMax) tau[pos][job] = tauMax;
        }
    }

    return best;
}
