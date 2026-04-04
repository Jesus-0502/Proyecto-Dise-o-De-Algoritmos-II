#include "ABC.h"
#include "../../Segundo-Corte/genetic-algorithm/Instances.h"

#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

static int reference_makespan(const string& name) {
    if (name == "pfsp_20x5") return 1278;
    if (name == "pfsp_20x10") return 1582;
    if (name == "pfsp_50x10") return 3037;
    if (name == "pfsp_100x10") return 5776;
    if (name == "pfsp_100x20") return 6330;
    return 0;
}

static double gap_percent(int value, int reference) {
    if (reference == 0) return 0.0;
    return 100.0 * (static_cast<double>(value) - static_cast<double>(reference)) /
           static_cast<double>(reference);
}

int main() {
    auto instances = get_taillard_benchmark_instances();

    vector<ABCParams> configs;

    {
        ABCParams p;
        p.colonySize = 80;
        p.eliteCount = 3;
        p.trialLimit = 16;
        p.localSearchPasses = 3;
        p.localSearchTrials = 10;
        p.onlookerCount = 40;
        p.relinkingPeriod = 14;
        p.scoutRandomRate = 0.40;
        p.guidedRefinementProb = 0.80;
        configs.push_back(p);
    }

    {
        ABCParams p;
        p.colonySize = 90;
        p.eliteCount = 3;
        p.trialLimit = 18;
        p.localSearchPasses = 4;
        p.localSearchTrials = 12;
        p.onlookerCount = 45;
        p.relinkingPeriod = 12;
        p.scoutRandomRate = 0.35;
        p.guidedRefinementProb = 0.85;
        configs.push_back(p);
    }

    {
        ABCParams p;
        p.colonySize = 100;
        p.eliteCount = 4;
        p.trialLimit = 24;
        p.localSearchPasses = 6;
        p.localSearchTrials = 18;
        p.onlookerCount = 60;
        p.relinkingPeriod = 8;
        p.scoutRandomRate = 0.25;
        p.guidedRefinementProb = 0.95;
        configs.push_back(p);
    }

    for (const auto& instance : instances) {
        int reference = reference_makespan(instance.name);

        cout << "Instancia: " << instance.name
             << " (n=" << instance.n
             << ", m=" << instance.m
             << ", seed=" << instance.seed << ")" << endl;

        for (size_t cfg = 0; cfg < configs.size(); ++cfg) {
            ABCParams p = configs[cfg];
            if (instance.n <= 20) {
                p.iterations = 220;
            } else if (instance.n <= 50) {
                p.iterations = 200;
            } else {
                p.iterations = 180;
            }
            p.seed = static_cast<unsigned int>(instance.seed);

            auto start = chrono::high_resolution_clock::now();
            ABCResult result = run_bee_colony_pfsp(instance.tiempos, instance.n, instance.m, p);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;

            cout << "  Config ABC #" << (cfg + 1)
                 << ": colony=" << p.colonySize
                 << ", elite=" << p.eliteCount
                 << ", trials=" << p.trialLimit
                 << ", LS=" << p.localSearchPasses
                 << ", LS-trials=" << p.localSearchTrials
                 << ", onlookers=" << p.onlookerCount
                 << ", relink=" << p.relinkingPeriod
                 << ", scout=" << p.scoutRandomRate
                 << ", guided=" << p.guidedRefinementProb
                 << ", iter=" << p.iterations << endl;

            cout << "    Mejor makespan: " << result.bestMakespan
                  << " | Gap: " << gap_percent(result.bestMakespan, reference) << "%" << endl;
            cout << "    Tiempo: " << elapsed.count() << " s" << endl;
            cout << "    Mejor secuencia: ";
            for (int job : result.bestSequence) {
                cout << (job + 1) << " ";
            }
            cout << endl;
        }

        cout << "----------------------------------------" << endl;
    }

    return 0;
}
