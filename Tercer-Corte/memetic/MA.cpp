#include "MA.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>

using namespace std;

int compute_makespan_ma(const vector<int>& secuencia,
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

static vector<int> initialize_individual(int n, mt19937& rng) {
    vector<int> ind(n);
    iota(ind.begin(), ind.end(), 0);
    shuffle(ind.begin(), ind.end(), rng);
    return ind;
}

static vector<int> improve_by_insertion_sampled(vector<int> seq,
                                                const vector<vector<int>>& tiempos,
                                                int m,
                                                int maxPasses,
                                                int maxTrialsPerPass,
                                                mt19937& rng) {
    if (seq.size() < 2) return seq;

    int n = static_cast<int>(seq.size());
    uniform_int_distribution<int> posDist(0, n - 1);

    for (int pass = 0; pass < maxPasses; ++pass) {
        int currentMs = compute_makespan_ma(seq, tiempos, m);
        bool improved = false;

        for (int t = 0; t < maxTrialsPerPass; ++t) {
            int i = posDist(rng);
            int j = posDist(rng);
            if (i == j) continue;

            vector<int> neighbor = seq;
            int job = neighbor[i];
            neighbor.erase(neighbor.begin() + i);
            neighbor.insert(neighbor.begin() + j, job);

            int ms = compute_makespan_ma(neighbor, tiempos, m);
            if (ms < currentMs) {
                seq = move(neighbor);
                currentMs = ms;
                improved = true;
                break;
            }
        }

        if (!improved) break;
    }

    return seq;
}

static int tournament_select(const vector<vector<int>>& population,
                             const vector<int>& makespans,
                             int tournamentSize,
                             mt19937& rng) {
    uniform_int_distribution<int> idxDist(0, static_cast<int>(population.size()) - 1);
    int bestIdx = idxDist(rng);

    for (int k = 1; k < tournamentSize; ++k) {
        int candidate = idxDist(rng);
        if (makespans[candidate] < makespans[bestIdx]) {
            bestIdx = candidate;
        }
    }
    return bestIdx;
}

static int next_unused_from_parent(const vector<int>& parent,
                                   int& cursor,
                                   const vector<char>& used) {
    int n = static_cast<int>(parent.size());
    while (cursor < n && used[parent[cursor]]) {
        ++cursor;
    }
    if (cursor >= n) return -1;
    return parent[cursor];
}

static int pick_consensus_or_best_incremental(const vector<int>& candidates,
                                              const vector<int>& partial,
                                              const vector<vector<int>>& tiempos,
                                              int m,
                                              mt19937& rng) {
    int n = static_cast<int>(candidates.size());
    if (n == 0) return -1;

    for (int i = 0; i < n; ++i) {
        int cnt = 1;
        for (int j = i + 1; j < n; ++j) {
            if (candidates[j] == candidates[i]) cnt++;
        }
        if (cnt >= 2) {
            return candidates[i];
        }
    }

    int bestJob = candidates[0];
    int bestMs = numeric_limits<int>::max();
    vector<int> tmp = partial;

    for (int job : candidates) {
        tmp.push_back(job);
        int ms = compute_makespan_ma(tmp, tiempos, m);
        if (ms < bestMs) {
            bestMs = ms;
            bestJob = job;
        } else if (ms == bestMs) {
            uniform_int_distribution<int> coin(0, 1);
            if (coin(rng) == 1) bestJob = job;
        }
        tmp.pop_back();
    }

    return bestJob;
}

static vector<int> multi_parent_recombination(const vector<int>& p1,
                                              const vector<int>& p2,
                                              const vector<int>& p3,
                                              const vector<vector<int>>& tiempos,
                                              int m,
                                              mt19937& rng) {
    int n = static_cast<int>(p1.size());
    vector<int> child;
    child.reserve(n);
    vector<char> used(n, false);

    int c1 = 0, c2 = 0, c3 = 0;

    for (int pos = 0; pos < n; ++pos) {
        int a = next_unused_from_parent(p1, c1, used);
        int b = next_unused_from_parent(p2, c2, used);
        int c = next_unused_from_parent(p3, c3, used);

        vector<int> candidates;
        if (a != -1) candidates.push_back(a);
        if (b != -1) candidates.push_back(b);
        if (c != -1) candidates.push_back(c);

        int chosen = pick_consensus_or_best_incremental(candidates, child, tiempos, m, rng);
        if (chosen == -1) {
            for (int job = 0; job < n; ++job) {
                if (!used[job]) {
                    chosen = job;
                    break;
                }
            }
        }

        child.push_back(chosen);
        used[chosen] = true;

        if (a == chosen) ++c1;
        if (b == chosen) ++c2;
        if (c == chosen) ++c3;
    }

    return child;
}

static void inversion_mutation(vector<int>& ind, mt19937& rng) {
    int n = static_cast<int>(ind.size());
    if (n < 2) return;

    uniform_int_distribution<int> dist(0, n - 1);
    int i = dist(rng);
    int j = dist(rng);
    if (i > j) swap(i, j);
    reverse(ind.begin() + i, ind.begin() + j + 1);
}

MAResult run_memetic_algorithm(const vector<vector<int>>& tiempos,
                               int n,
                               int m,
                               const MAParams& params) {
    mt19937 rng(params.seed == 0 ? random_device{}() : params.seed);

    vector<vector<int>> population;
    population.reserve(params.populationSize);

    for (int i = 0; i < params.populationSize; ++i) {
        population.push_back(initialize_individual(n, rng));
    }

    vector<int> makespans(params.populationSize, 0);

    MAResult best;
    best.bestMakespan = numeric_limits<int>::max();

    auto evaluate = [&]() {
        for (int i = 0; i < params.populationSize; ++i) {
            makespans[i] = compute_makespan_ma(population[i], tiempos, m);
            if (makespans[i] < best.bestMakespan) {
                best.bestMakespan = makespans[i];
                best.bestSequence = population[i];
            }
        }
    };

    evaluate();

    uniform_real_distribution<double> prob(0.0, 1.0);

    for (int iter = 0; iter < params.iterations; ++iter) {
        vector<int> order(params.populationSize);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            return makespans[a] < makespans[b];
        });

        vector<vector<int>> newPopulation;
        newPopulation.reserve(params.populationSize);

        int elites = min(params.eliteCount, params.populationSize);
        for (int e = 0; e < elites; ++e) {
            newPopulation.push_back(population[order[e]]);
        }

        while (static_cast<int>(newPopulation.size()) < params.populationSize) {
            int i1 = tournament_select(population, makespans, params.tournamentSize, rng);
            int i2 = tournament_select(population, makespans, params.tournamentSize, rng);
            int i3 = tournament_select(population, makespans, params.tournamentSize, rng);

            vector<int> child;
            if (prob(rng) <= params.recombinationProb) {
                child = multi_parent_recombination(population[i1], population[i2], population[i3], tiempos, m, rng);
            } else {
                child = population[i1];
            }

            if (prob(rng) <= params.mutationProb) {
                inversion_mutation(child, rng);
            }

            int maxTrials = min(6 * n, 180);
            child = improve_by_insertion_sampled(child, tiempos, m, params.localSearchIters, maxTrials, rng);
            newPopulation.push_back(child);
        }

        population.swap(newPopulation);
        evaluate();
    }

    return best;
}
