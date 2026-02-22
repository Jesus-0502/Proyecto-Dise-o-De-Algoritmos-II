#include "GA.h"
#include "Instances.h"

#include <chrono>
#include <iostream>

using namespace std;

int main() {
    auto instances = get_taillard_benchmark_instances();

    for (const auto& instance : instances) {
        GAParams params;
        params.populationSize = 150;
        params.crossoverProb = 0.85;
        params.mutationProb = 0.15;
        params.iterations = 150 * (instance.n + instance.m);
        params.seed = static_cast<unsigned int>(chrono::high_resolution_clock::now().time_since_epoch().count());

        auto start = chrono::high_resolution_clock::now();
        GAResult result = run_genetic_algorithm(instance.tiempos, instance.n, instance.m, params);
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;

        cout << "Instancia: " << instance.name << " (n=" << instance.n << ", m=" << instance.m << ")" << endl;
        cout << "Parametros GA: poblacion=" << params.populationSize
             << ", pc=" << params.crossoverProb
             << ", pm=" << params.mutationProb
             << ", iteraciones=" << params.iterations << endl;
        cout << "Mejor makespan: " << result.bestMakespan << endl;
        cout << "Mejor secuencia: ";
        for (int job : result.bestSequence) {
            cout << job + 1 << " ";
        }
        cout << "\nTiempo: " << elapsed.count() << " segundos" << endl;
        cout << "----------------------------------------" << endl;
    }

    return 0;
}