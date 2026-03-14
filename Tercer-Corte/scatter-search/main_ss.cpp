#include "SS.h"
#include "../../Segundo-Corte/genetic-algorithm/Instances.h"

#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    auto instances = get_taillard_benchmark_instances();

    vector<SSParams> configs;
    {
        SSParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.9;
        p.mutationProb = 0.6;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        p.diversityThreshold = 25;
        p.pathRelinkingRate = 0.20;
        configs.push_back(p);
    }
    {
        SSParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.9;
        p.mutationProb = 0.6;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        p.diversityThreshold = 25;
        p.pathRelinkingRate = 0.30;
        configs.push_back(p);
    }
        {
        SSParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.9;
        p.mutationProb = 0.6;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        p.diversityThreshold = 25;
        p.pathRelinkingRate = 0.40;
        configs.push_back(p);
    }
        {
        SSParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.9;
        p.mutationProb = 0.6;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        p.diversityThreshold = 25;
        p.pathRelinkingRate = 0.50;
        configs.push_back(p);
    }
        {
        SSParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.9;
        p.mutationProb = 0.6;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        p.diversityThreshold = 25;
        p.pathRelinkingRate = 0.60;
        configs.push_back(p);
    }

    for (const auto& instance : instances) {
        cout << "Instancia: " << instance.name
             << " (n=" << instance.n
             << ", m=" << instance.m
             << ", seed=" << instance.seed << ")" << endl;

        for (size_t cfg = 0; cfg < configs.size(); ++cfg) {
            SSParams p = configs[cfg];
            if (instance.n <= 20) p.iterations = 220;
            else if (instance.n <= 50) p.iterations = 220;
            else p.iterations = 220;
            p.seed = static_cast<unsigned int>(instance.seed);

            auto start = chrono::high_resolution_clock::now();
            SSResult result = run_memetic_algorithm(instance.tiempos, instance.n, instance.m, p);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;

            cout << "  Config SS #" << (cfg + 1)
                 << ": pop=" << p.populationSize
                 << ", pr=" << p.recombinationProb
                 << ", pm=" << p.mutationProb
                 << ", torneo=" << p.tournamentSize
                 << ", LS=" << p.localSearchIters
                 << ", iter=" << p.iterations
                 << ", threshold=" << p.diversityThreshold
                 << ", pathRelinkingRate=" << p.pathRelinkingRate << endl;

            cout << "    Mejor makespan: " << result.bestMakespan << endl;
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
