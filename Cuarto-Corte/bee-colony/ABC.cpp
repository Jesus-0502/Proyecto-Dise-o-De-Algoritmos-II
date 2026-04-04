#include "ABC.h"
#include "../../Segundo-Corte/genetic-algorithm/Instances.h"

#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <numeric>
#include <random>
#include <vector>

using namespace std;

int compute_makespan_abc(const vector<int>& secuencia,
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

static vector<int> make_random_permutation(int n, mt19937& rng) {
    vector<int> perm(n);
    iota(perm.begin(), perm.end(), 0);
    shuffle(perm.begin(), perm.end(), rng);
    return perm;
}

static vector<int> neh_sequence(const vector<vector<int>>& tiempos,
                                int n,
                                int m) {
    vector<int> jobs(n);
    iota(jobs.begin(), jobs.end(), 0);

    vector<int> totals(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            totals[i] += tiempos[i][j];
        }
    }

    sort(jobs.begin(), jobs.end(), [&](int a, int b) {
        if (totals[a] != totals[b]) return totals[a] > totals[b];
        return a < b;
    });

    vector<int> seq;
    seq.reserve(n);

    for (int job : jobs) {
        int bestPos = 0;
        int bestMs = numeric_limits<int>::max();

        for (int pos = 0; pos <= static_cast<int>(seq.size()); ++pos) {
            vector<int> candidate = seq;
            candidate.insert(candidate.begin() + pos, job);
            int ms = compute_makespan_abc(candidate, tiempos, m);
            if (ms < bestMs) {
                bestMs = ms;
                bestPos = pos;
            }
        }

        seq.insert(seq.begin() + bestPos, job);
    }

    return seq;
}

static void apply_random_insertion(vector<int>& seq, mt19937& rng) {
    int n = static_cast<int>(seq.size());
    if (n < 2) return;

    uniform_int_distribution<int> dist(0, n - 1);
    int i = dist(rng);
    int j = dist(rng);
    if (i == j) return;

    int job = seq[i];
    seq.erase(seq.begin() + i);
    if (j > i) --j;
    seq.insert(seq.begin() + j, job);
}

static void apply_random_inversion(vector<int>& seq, mt19937& rng) {
    int n = static_cast<int>(seq.size());
    if (n < 2) return;

    uniform_int_distribution<int> dist(0, n - 1);
    int i = dist(rng);
    int j = dist(rng);
    if (i > j) swap(i, j);
    if (i == j) return;

    reverse(seq.begin() + i, seq.begin() + j + 1);
}

static vector<int> randomized_neh_sequence(const vector<vector<int>>& tiempos,
                                           int n,
                                           int m,
                                           mt19937& rng) {
    vector<int> seq = neh_sequence(tiempos, n, m);
    int perturbations = max(1, n / 10);

    uniform_real_distribution<double> prob(0.0, 1.0);
    for (int p = 0; p < perturbations; ++p) {
        if (prob(rng) < 0.55) {
            apply_random_insertion(seq, rng);
        } else {
            apply_random_inversion(seq, rng);
        }
    }

    return seq;
}

static vector<int> local_search_insertion_sampled(vector<int> seq,
                                                 const vector<vector<int>>& tiempos,
                                                 int m,
                                                 int maxPasses,
                                                 int maxTrialsPerPass,
                                                 mt19937& rng) {
    if (seq.size() < 2) return seq;

    int n = static_cast<int>(seq.size());
    uniform_int_distribution<int> posDist(0, n - 1);

    for (int pass = 0; pass < maxPasses; ++pass) {
        int currentMs = compute_makespan_abc(seq, tiempos, m);
        bool improved = false;

        for (int trial = 0; trial < maxTrialsPerPass; ++trial) {
            int i = posDist(rng);
            int j = posDist(rng);
            if (i == j) continue;

            vector<int> neighbor = seq;
            int job = neighbor[i];
            neighbor.erase(neighbor.begin() + i);
            if (j > i) --j;
            neighbor.insert(neighbor.begin() + j, job);

            int ms = compute_makespan_abc(neighbor, tiempos, m);
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

static vector<int> guided_neighbor(const vector<int>& source,
                                   const vector<int>& reference,
                                   mt19937& rng) {
    vector<int> child = source;
    int n = static_cast<int>(child.size());
    if (n < 2) return child;

    vector<int> refPos(n, 0);
    for (int i = 0; i < n; ++i) {
        refPos[reference[i]] = i;
    }

    int bestIdx = 0;
    int bestDist = -1;
    for (int i = 0; i < n; ++i) {
        int job = child[i];
        int dist = abs(i - refPos[job]);
        if (dist > bestDist) {
            bestDist = dist;
            bestIdx = i;
        } else if (dist == bestDist) {
            uniform_int_distribution<int> coin(0, 1);
            if (coin(rng) == 1) bestIdx = i;
        }
    }

    int pos = bestIdx;
    int job = child[pos];
    int targetPos = refPos[job];
    if (targetPos > pos) --targetPos;
    if (targetPos != pos) {
        child.erase(child.begin() + pos);
        child.insert(child.begin() + targetPos, job);
    }

    uniform_real_distribution<double> prob(0.0, 1.0);
    if (prob(rng) < 0.45) {
        apply_random_inversion(child, rng);
    } else if (prob(rng) < 0.75) {
        apply_random_insertion(child, rng);
    }

    return child;
}

static vector<int> path_relink_best_of_path(const vector<int>& source,
                                            const vector<int>& target,
                                            const vector<vector<int>>& tiempos,
                                            int m,
                                            mt19937& rng) {
    vector<int> current = source;
    vector<int> best = current;
    int bestMs = compute_makespan_abc(current, tiempos, m);

    int n = static_cast<int>(current.size());
    if (n < 2) return best;

    vector<int> targetPos(n, 0);
    for (int i = 0; i < n; ++i) {
        targetPos[target[i]] = i;
    }

    uniform_int_distribution<int> coin(0, 1);

    for (int step = 0; step < n; ++step) {
        int chosenIdx = -1;
        int chosenDist = -1;

        for (int i = 0; i < n; ++i) {
            int job = current[i];
            int desiredPos = targetPos[job];
            if (desiredPos == i) continue;

            int dist = abs(i - desiredPos);
            if (dist > chosenDist) {
                chosenDist = dist;
                chosenIdx = i;
            } else if (dist == chosenDist && chosenIdx != -1 && coin(rng) == 1) {
                chosenIdx = i;
            }
        }

        if (chosenIdx == -1) break;

        int job = current[chosenIdx];
        int insertPos = targetPos[job];
        if (insertPos > chosenIdx) --insertPos;

        current.erase(current.begin() + chosenIdx);
        current.insert(current.begin() + insertPos, job);

        int ms = compute_makespan_abc(current, tiempos, m);
        if (ms < bestMs) {
            bestMs = ms;
            best = current;
        }

        if (current == target) break;
    }

    return best;
}

static int roulette_select(const vector<int>& makespans,
                           const vector<int>& candidates,
                           mt19937& rng) {
    double total = 0.0;
    vector<double> weights;
    weights.reserve(candidates.size());

    for (int idx : candidates) {
        double weight = 1.0 / (1.0 + static_cast<double>(makespans[idx]));
        weights.push_back(weight);
        total += weight;
    }

    uniform_real_distribution<double> pick(0.0, total);
    double target = pick(rng);

    double cumulative = 0.0;
    for (size_t i = 0; i < candidates.size(); ++i) {
        cumulative += weights[i];
        if (cumulative >= target) return candidates[i];
    }

    return candidates.back();
}

ABCResult run_bee_colony_pfsp(const vector<vector<int>>& tiempos,
                              int n,
                              int m,
                              const ABCParams& params) {
    mt19937 rng(params.seed == 0 ? random_device{}() : params.seed);

    int colonySize = max(4, params.colonySize);
    int eliteCount = max(1, min(params.eliteCount, colonySize - 1));
    int onlookerCount = params.onlookerCount > 0 ? params.onlookerCount : max(1, colonySize / 2);
    int trialLimit = max(1, params.trialLimit);
    int localPasses = max(1, params.localSearchPasses);
    int localTrials = max(1, params.localSearchTrials);
    int relinkingPeriod = max(1, params.relinkingPeriod);

    vector<vector<int>> population;
    population.reserve(colonySize);

    vector<int> makespans(colonySize, 0);
    vector<int> trials(colonySize, 0);

    for (int i = 0; i < colonySize; ++i) {
        vector<int> seedSeq;
        if (i < (colonySize * 3) / 4) {
            seedSeq = randomized_neh_sequence(tiempos, n, m, rng);
        } else {
            seedSeq = make_random_permutation(n, rng);
        }

        seedSeq = local_search_insertion_sampled(move(seedSeq), tiempos, m,
                                                 max(1, localPasses - 2),
                                                 max(3, localTrials / 2),
                                                 rng);
        population.push_back(move(seedSeq));
    }

    ABCResult best;
    best.bestMakespan = numeric_limits<int>::max();

    auto evaluate = [&]() {
        for (int i = 0; i < colonySize; ++i) {
            makespans[i] = compute_makespan_abc(population[i], tiempos, m);
            if (makespans[i] < best.bestMakespan) {
                best.bestMakespan = makespans[i];
                best.bestSequence = population[i];
            }
        }
    };

    evaluate();

    uniform_real_distribution<double> prob(0.0, 1.0);

    for (int iter = 0; iter < params.iterations; ++iter) {
        vector<int> order(colonySize);
        iota(order.begin(), order.end(), 0);
        sort(order.begin(), order.end(), [&](int a, int b) {
            return makespans[a] < makespans[b];
        });

        int bestIndex = order.front();
        int referenceBand = max(eliteCount + 1, colonySize / 4);
        referenceBand = min(referenceBand, colonySize);

        // Employed bees: cada fuente intenta mejorar guiándose por una solución elite.
        for (int rank = eliteCount; rank < colonySize; ++rank) {
            int idx = order[rank];
            int refIdx = order[uniform_int_distribution<int>(0, referenceBand - 1)(rng)];

            vector<int> candidate = guided_neighbor(population[idx], population[refIdx], rng);
            if (prob(rng) < params.guidedRefinementProb) {
                candidate = local_search_insertion_sampled(move(candidate), tiempos, m,
                                                           localPasses, localTrials, rng);
            }

            int ms = compute_makespan_abc(candidate, tiempos, m);
            if (ms < makespans[idx]) {
                population[idx] = move(candidate);
                makespans[idx] = ms;
                trials[idx] = 0;
            } else {
                ++trials[idx];
            }

            if (makespans[idx] < best.bestMakespan) {
                best.bestMakespan = makespans[idx];
                best.bestSequence = population[idx];
                bestIndex = idx;
            }
        }

        // Onlooker bees: reforzar las soluciones más prometedoras.
        vector<int> rouletteCandidates(colonySize);
        iota(rouletteCandidates.begin(), rouletteCandidates.end(), 0);

        for (int o = 0; o < onlookerCount; ++o) {
            int idx = roulette_select(makespans, rouletteCandidates, rng);
            int refIdx = (prob(rng) < 0.7) ? bestIndex
                                           : order[uniform_int_distribution<int>(0, referenceBand - 1)(rng)];

            vector<int> candidate = guided_neighbor(population[idx], population[refIdx], rng);
            candidate = local_search_insertion_sampled(move(candidate), tiempos, m,
                                                       localPasses,
                                                       localTrials, rng);

            int ms = compute_makespan_abc(candidate, tiempos, m);
            if (ms < makespans[idx]) {
                population[idx] = move(candidate);
                makespans[idx] = ms;
                trials[idx] = 0;
            } else {
                ++trials[idx];
            }

            if (makespans[idx] < best.bestMakespan) {
                best.bestMakespan = makespans[idx];
                best.bestSequence = population[idx];
                bestIndex = idx;
            }
        }

        // Scout bees: reinyectar diversidad cuando una fuente se estanca.
        for (int i = 0; i < colonySize; ++i) {
            if (trials[i] >= trialLimit) {
                if (prob(rng) < params.scoutRandomRate) {
                    population[i] = make_random_permutation(n, rng);
                } else {
                    population[i] = randomized_neh_sequence(tiempos, n, m, rng);
                }
                population[i] = local_search_insertion_sampled(move(population[i]), tiempos, m,
                                                                  max(1, localPasses - 2),
                                                                  localTrials, rng);
                makespans[i] = compute_makespan_abc(population[i], tiempos, m);
                trials[i] = 0;

                if (makespans[i] < best.bestMakespan) {
                    best.bestMakespan = makespans[i];
                    best.bestSequence = population[i];
                    bestIndex = i;
                }
            }
        }

        // Path relinking periódico entre la mejor fuente y una elite distinta.
        if ((iter + 1) % relinkingPeriod == 0 && colonySize > eliteCount) {
            int referenceIdx = order[uniform_int_distribution<int>(0, referenceBand - 1)(rng)];
            if (referenceIdx == bestIndex && colonySize > eliteCount + 1) {
                referenceIdx = order[uniform_int_distribution<int>(1, referenceBand - 1)(rng)];
            }

            vector<int> candidate = path_relink_best_of_path(population[bestIndex],
                                                             population[referenceIdx],
                                                             tiempos, m, rng);
            candidate = local_search_insertion_sampled(move(candidate), tiempos, m,
                                                       localPasses,
                                                       localTrials, rng);

            int ms = compute_makespan_abc(candidate, tiempos, m);
            int worstIdx = order.back();
            if (ms < makespans[worstIdx]) {
                population[worstIdx] = move(candidate);
                makespans[worstIdx] = ms;
                trials[worstIdx] = 0;
            }

            if (ms < best.bestMakespan) {
                best.bestMakespan = ms;
                best.bestSequence = population[worstIdx];
                bestIndex = worstIdx;
            }
        }
    }

    return best;
}
