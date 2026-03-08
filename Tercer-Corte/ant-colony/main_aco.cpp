#include "ACO.h"
#include "../../Segundo-Corte/genetic-algorithm/Instances.h"

#include <chrono>
#include <iostream>
#include <vector>

using namespace std;

int main() {
    auto instances = get_taillard_benchmark_instances();

    vector<ACOParams> configs;
    {
        ACOParams p;
        p.antCount = 25;
        p.alpha = 1.0;
        p.beta = 2.0;
        p.rho = 0.15;
        p.q0 = 0.85;
        p.Q = 100.0;
        configs.push_back(p);
    }
    {
        ACOParams p;
        p.antCount = 35;
        p.alpha = 1.1;
        p.beta = 2.5;
        p.rho = 0.20;
        p.q0 = 0.90;
        p.Q = 120.0;
        configs.push_back(p);
    }

    for (const auto& instance : instances) {
        cout << "Instancia: " << instance.name
             << " (n=" << instance.n
             << ", m=" << instance.m
             << ", seed=" << instance.seed << ")" << endl;

        for (size_t cfg = 0; cfg < configs.size(); ++cfg) {
            ACOParams p = configs[cfg];
            if (instance.n <= 20) p.iterations = 220;
            else if (instance.n <= 50) p.iterations = 180;
            else p.iterations = 120;
            p.seed = static_cast<unsigned int>(instance.seed);

            auto start = chrono::high_resolution_clock::now();
            ACOResult result = run_aco_pfsp(instance.tiempos, instance.n, instance.m, p);
            auto end = chrono::high_resolution_clock::now();
            chrono::duration<double> elapsed = end - start;

            cout << "  Config ACO #" << (cfg + 1)
                 << ": ants=" << p.antCount
                 << ", alpha=" << p.alpha
                 << ", beta=" << p.beta
                 << ", rho=" << p.rho
                 << ", q0=" << p.q0
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
