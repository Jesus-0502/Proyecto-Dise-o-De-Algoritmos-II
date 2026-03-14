#include "MA.h"
#include "../../Segundo-Corte/genetic-algorithm/Instances.h"

#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    auto instances = get_taillard_benchmark_instances();

    vector<MAParams> configs;
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 1.0;
        p.mutationProb = 0.50;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.90;
        p.mutationProb = 0.60;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.80;
        p.mutationProb = 0.70;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.70;
        p.mutationProb = 0.80;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.60;
        p.mutationProb = 0.90;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }
    {
        MAParams p;
        p.populationSize = 100;
        p.recombinationProb = 0.50;
        p.mutationProb = 1.0;
        p.tournamentSize = 5;
        p.localSearchIters = 30;
        p.eliteCount = 3;
        configs.push_back(p);
    }

    for (const auto& instance : instances) {
        cout << "Instancia: " << instance.name
             << " (n=" << instance.n
             << ", m=" << instance.m
             << ", seed=" << instance.seed << ")" << endl;

        for (size_t cfg = 0; cfg < configs.size(); ++cfg) {
            MAParams p = configs[cfg];
            if (instance.n <= 20) p.iterations = 220;
            else if (instance.n <= 50) p.iterations = 160;
            else p.iterations = 90;
            p.seed = static_cast<unsigned int>(instance.seed);

            auto start = chrono::high_resolution_clock::now();
            MAResult result = run_memetic_algorithm(instance.tiempos, instance.n, instance.m, p);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;

            cout << "  Config MA #" << (cfg + 1)
                 << ": pop=" << p.populationSize
                 << ", pr=" << p.recombinationProb
                 << ", pm=" << p.mutationProb
                 << ", torneo=" << p.tournamentSize
                 << ", LS=" << p.localSearchIters
                 << ", iter=" << p.iterations << endl;

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
