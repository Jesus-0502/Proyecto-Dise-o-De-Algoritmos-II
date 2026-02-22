#include "grasp.h"
#include "Instances.h"

#include <chrono>
#include <iostream>

using namespace std;

int main() {
    auto instances = get_taillard_benchmark_instances();

    for (const auto& instance : instances) {

        int iteraciones_grasp = 50;
        double alphas[] = {0.1, 0.3, 0.5, 0.7, 0.9};
        cout << "Instancia: " << instance.name << " (n=" << instance.n << ", m=" << instance.m << ", seed=" << instance.seed << ")" << endl;
        for (double alpha : alphas) {

            auto inicio_grasp = chrono::high_resolution_clock::now();
            vector<int> secuencia_grasp = ejecutarGRASP(instance.n, instance.m, instance.tiempos, iteraciones_grasp, alpha);
            auto fin_grasp = chrono::high_resolution_clock::now();
            chrono::duration<double> tiempo_grasp = fin_grasp - inicio_grasp;

            cout << "Secuencia GRASP (Alpha=" << alpha << "): ";
            for (int t : secuencia_grasp) cout << t+1 << " ";
            cout << "\nMakespan GRASP: " << calcular_makespan(secuencia_grasp, instance.tiempos, instance.m) << endl;
            cout << "Tiempo GRASP: " << tiempo_grasp.count() << "s" << endl;
            cout << "-------------------" << endl;
        }
    }

    return 0;
}
