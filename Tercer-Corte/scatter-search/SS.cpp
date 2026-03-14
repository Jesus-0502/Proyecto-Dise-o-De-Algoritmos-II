#include "SS.h"
#include <iostream>
#include <algorithm>
#include <limits>
#include <numeric>
#include <random>
#include <unordered_set>
#include <vector>

using namespace std;

static int solution_distance(const vector<int>& a, const vector<int>& b) {

    int d = 0;
    int n = a.size();

    for(int i = 0; i < n; i++){
        if(a[i] != b[i]) d++;
    }

    return d;
}

static bool is_diverse(const vector<int>& candidate,
                       const vector<vector<int>>& population,
                       int threshold){

    for(const auto& sol : population){

        if(solution_distance(candidate, sol) < threshold){
            return false;
        }
    }

    return true;
}

static vector<int> path_relinking(const vector<int>& start,
                                  const vector<int>& target,
                                  const vector<vector<int>>& tiempos,
                                  int m){

    vector<int> current = start;
    vector<int> best = start;

    int bestMs = compute_makespan_ma(best, tiempos, m);

    int n = start.size();

    for(int i = 0; i < n; i++){

        if(current[i] == target[i])
            continue;

        int job = target[i];

        int pos = -1;
        for(int j = 0; j < n; j++){
            if(current[j] == job){
                pos = j;
                break;
            }
        }

        swap(current[i], current[pos]);

        int ms = compute_makespan_ma(current, tiempos, m);

        if(ms < bestMs){
            bestMs = ms;
            best = current;
        }
    }

    return best;
}

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

// static int next_unused_from_parent(const vector<int>& parent,
//                                    int& cursor,
//                                    const vector<char>& used) {
//     int n = static_cast<int>(parent.size());
//     while (cursor < n && used[parent[cursor]]) {
//         ++cursor;
//     }
//     if (cursor >= n) return -1;
//     return parent[cursor];
// }

// static int pick_consensus_or_best_incremental(const vector<int>& candidates,
//                                               const vector<int>& partial,
//                                               const vector<vector<int>>& tiempos,
//                                               int m,
//                                               mt19937& rng) {
//     int n = static_cast<int>(candidates.size());
//     if (n == 0) return -1;

//     for (int i = 0; i < n; ++i) {
//         int cnt = 1;
//         for (int j = i + 1; j < n; ++j) {
//             if (candidates[j] == candidates[i]) cnt++;
//         }
//         if (cnt >= 2) {
//             return candidates[i];
//         }
//     }

//     int bestJob = candidates[0];
//     int bestMs = numeric_limits<int>::max();
//     vector<int> tmp = partial;

//     for (int job : candidates) {
//         tmp.push_back(job);
//         int ms = compute_makespan_ma(tmp, tiempos, m);
//         if (ms < bestMs) {
//             bestMs = ms;
//             bestJob = job;
//         } else if (ms == bestMs) {
//             uniform_int_distribution<int> coin(0, 1);
//             if (coin(rng) == 1) bestJob = job;
//         }
//         tmp.pop_back();
//     }

//     return bestJob;
// }

static vector<int> sjox_three_parents(const vector<int>& p1,
                                      const vector<int>& p2,
                                      const vector<int>& p3) {

    int n = p1.size();
    vector<int> child(n, -1);
    vector<char> used(n, false);

    // Step 1: detectar consenso entre padres
    for (int i = 0; i < n; ++i) {

        int a = p1[i];
        int b = p2[i];
        int c = p3[i];

        if (a == b || a == c) {
            child[i] = a;
            used[a] = true;
        }
        else if (b == c) {
            child[i] = b;
            used[b] = true;
        }
    }

    // Step 2: rellenar huecos con orden relativo
    int cursor = 0;

    for (int i = 0; i < n; ++i) {

        if (child[i] != -1)
            continue;

        while (cursor < n && used[p1[cursor]])
            cursor++;

        if (cursor < n) {
            child[i] = p1[cursor];
            used[p1[cursor]] = true;
            cursor++;
        }
    }

    // Safety check
    for (int i = 0; i < n; ++i) {
        if (child[i] == -1) {
            for (int j = 0; j < n; ++j) {
                if (!used[j]) {
                    child[i] = j;
                    used[j] = true;
                    break;
                }
            }
        }
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

// static void remove_duplicates(vector<vector<int>>& population,
//                               mt19937& rng) {

//     unordered_set<string> seen;

//     for (auto& ind : population) {

//         string key;
//         key.reserve(ind.size() * 3);

//         for (int x : ind) {
//             key += to_string(x);
//             key += ",";
//         }

//         if (seen.count(key)) {

//             // regenerar individuo
//             iota(ind.begin(), ind.end(), 0);
//             shuffle(ind.begin(), ind.end(), rng);
//         }

//         key.clear();

//         for (int x : ind) {
//             key += to_string(x);
//             key += ",";
//         }

//         seen.insert(key);
//     }
// }

SSResult run_memetic_algorithm(const vector<vector<int>>& tiempos,
                               int n,
                               int m,
                               const SSParams& params) {
    mt19937 rng(params.seed == 0 ? random_device{}() : params.seed);

    vector<vector<int>> population;
    population.reserve(params.populationSize);

    for (int i = 0; i < params.populationSize; ++i) {
        population.push_back(initialize_individual(n, rng));
    }

    vector<int> makespans(params.populationSize, 0);

    SSResult best;
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
                // child = multi_parent_recombination(population[i1], population[i2], population[i3], tiempos, m, rng);
                child = sjox_three_parents(population[i1], population[i2], population[i3]);
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

        int numPairs = population.size() * params.pathRelinkingRate;


        // int totalPairs = population.size() * (population.size() - 1) / 2;
        // int numPairs = totalPairs * params.pathRelinkingRate;


        uniform_int_distribution<int> dist(0, population.size()-1);

        for(int k = 0; k < numPairs; k++){

            int i = dist(rng);
            int j = dist(rng);

            if(i == j) continue;

            vector<int> pr = path_relinking(population[i],
                                            population[j],
                                            tiempos,
                                            m);

            if(is_diverse(pr, population, params.diversityThreshold)){
                population.push_back(pr);
            }
        }

        evaluate();

        if(int(population.size()) > params.populationSize){

        vector<int> order(population.size());
        iota(order.begin(), order.end(), 0);

        vector<int> ms(population.size());

        for(int i = 0; i < int(population.size()); i++){
            ms[i] = compute_makespan_ma(population[i], tiempos, m);
        }

        sort(order.begin(), order.end(),
            [&](int a, int b){ return ms[a] < ms[b]; });

        vector<vector<int>> trimmed;

        for(int i = 0; i < params.populationSize; i++){
            trimmed.push_back(population[order[i]]);
        }

        population.swap(trimmed);
    }

    }

    return best;
}
