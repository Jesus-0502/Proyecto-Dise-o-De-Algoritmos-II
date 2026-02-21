#include "GA.h"

#include <algorithm>
#include <limits>
#include <numeric>
#include <random>

using namespace std;

static int calcular_makespan(const vector<int>& secuencia,
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

static double fitness_from_makespan(int makespan) {
    return 1.0 / (1.0 + static_cast<double>(makespan));
}

static vector<int> repair_permutation(const vector<int>& individual, int n) {
    vector<int> repaired = individual;
    vector<int> count(n, 0);
    for (int job : repaired) {
        if (job >= 0 && job < n) {
            count[job]++;
        }
    }

    vector<int> missing;
    missing.reserve(n);
    for (int job = 0; job < n; ++job) {
        if (count[job] == 0) {
            missing.push_back(job);
        }
    }

    int missing_idx = 0;
    for (int i = 0; i < n; ++i) {
        int job = repaired[i];
        if (job < 0 || job >= n || count[job] > 1) {
            if (job >= 0 && job < n) {
                count[job]--;
            }
            repaired[i] = missing[missing_idx++];
        }
    }

    return repaired;
}

static void inversion_mutation(vector<int>& individual, mt19937& rng) {
    int n = static_cast<int>(individual.size());
    if (n < 2) return;
    uniform_int_distribution<int> dist(0, n - 1);
    int i = dist(rng);
    int j = dist(rng);
    if (i > j) swap(i, j);
    reverse(individual.begin() + i, individual.begin() + j + 1);
}

GAResult run_genetic_algorithm(const vector<vector<int>>& tiempos,
                               int n,
                               int m,
                               const GAParams& params) {
    mt19937 rng(params.seed == 0 ? random_device{}() : params.seed);
    vector<int> base(n);
    iota(base.begin(), base.end(), 0);

    vector<vector<int>> population;
    population.reserve(params.populationSize);
    for (int i = 0; i < params.populationSize; ++i) {
        vector<int> ind = base;
        shuffle(ind.begin(), ind.end(), rng);
        population.push_back(ind);
    }

    vector<double> fitness(params.populationSize, 0.0);
    vector<int> makespans(params.populationSize, 0);

    GAResult best;
    best.bestMakespan = numeric_limits<int>::max();

    auto evaluate_population = [&]() {
        for (int i = 0; i < params.populationSize; ++i) {
            makespans[i] = calcular_makespan(population[i], tiempos, m);
            fitness[i] = fitness_from_makespan(makespans[i]);
            if (makespans[i] < best.bestMakespan) {
                best.bestMakespan = makespans[i];
                best.bestSequence = population[i];
            }
        }
    };

    evaluate_population();

    uniform_real_distribution<double> prob(0.0, 1.0);
    uniform_int_distribution<int> distIndex(0, n - 1);

    for (int iter = 0; iter < params.iterations; ++iter) {
        vector<double> cumulative(params.populationSize, 0.0);
        cumulative[0] = fitness[0];
        for (int i = 1; i < params.populationSize; ++i) {
            cumulative[i] = cumulative[i - 1] + fitness[i];
        }

        auto select_parent = [&]() -> const vector<int>& {
            uniform_real_distribution<double> dist(0.0, cumulative.back());
            double r = dist(rng);
            auto it = lower_bound(cumulative.begin(), cumulative.end(), r);
            int idx = static_cast<int>(distance(cumulative.begin(), it));
            return population[idx];
        };

        vector<vector<int>> new_population;
        new_population.reserve(params.populationSize);

        while (static_cast<int>(new_population.size()) < params.populationSize) {
            const vector<int>& p1 = select_parent();
            const vector<int>& p2 = select_parent();

            vector<int> c1 = p1;
            vector<int> c2 = p2;

            if (prob(rng) <= params.crossoverProb) {
                int a = distIndex(rng);
                int b = distIndex(rng);
                if (a > b) swap(a, b);

                for (int i = a; i <= b; ++i) {
                    c1[i] = p2[i];
                    c2[i] = p1[i];
                }

                c1 = repair_permutation(c1, n);
                c2 = repair_permutation(c2, n);
            }

            if (prob(rng) <= params.mutationProb) {
                inversion_mutation(c1, rng);
            }
            if (prob(rng) <= params.mutationProb) {
                inversion_mutation(c2, rng);
            }

            new_population.push_back(c1);
            if (static_cast<int>(new_population.size()) < params.populationSize) {
                new_population.push_back(c2);
            }
        }

        population.swap(new_population);
        evaluate_population();
    }

    return best;
}
