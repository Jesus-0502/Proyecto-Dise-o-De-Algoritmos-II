#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <bits/stdc++.h>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include "Instances.h"

#include <chrono>
#include <iostream>

using namespace std;
const int N = 1000000;
const int N_t = 10;
const double boltzmann = 1.0/(log(1.0/0.99));

// Función para calcular el makespan (Cmax)
double calcular_makespan(const vector<int>& secuencia, const vector<vector<int>>& tiempos, int n_maquinas) {
    int n_tareas = secuencia.size();
    if (n_tareas == 0) return 0;

    vector<vector<double>> C(n_tareas + 1, vector<double>(n_maquinas + 1, 0));
    for (int i = 1; i <= n_tareas; ++i) {
        for (int j = 1; j <= n_maquinas; ++j) {
            C[i][j] = max(C[i-1][j], C[i][j-1]) + tiempos[secuencia[i-1]][j-1];
        }
    }
    return C[n_tareas][n_maquinas];
}

vector<int> neh(int n, int m, const vector<vector<int>>& tiempos) {
    // Ordenar tareas por suma total de tiempos
    vector<pair<int, int>> sum_tiempos;
    for (int i = 0; i < n; ++i) {
        int sum = accumulate(tiempos[i].begin(), tiempos[i].end(), 0);
        sum_tiempos.push_back({sum, i});
    }
    sort(sum_tiempos.begin(), sum_tiempos.end(), greater<pair<int, int>>());

    vector<int> secuencia_actual;
    int mejor_makespan_final = 0;

    //Inserción iterativa de tareas
    for (int i = 0; i < n; ++i) {
        int tarea_actual = sum_tiempos[i].second;
        int mejor_makespan_iteracion = 2e9;
        vector<int> mejor_secuencia_iteracion;

        for (int pos = 0; pos <= (int)secuencia_actual.size(); ++pos) {
            vector<int> temp_secuencia = secuencia_actual;
            temp_secuencia.insert(temp_secuencia.begin() + pos, tarea_actual);

            int ms = calcular_makespan(temp_secuencia, tiempos, m);
            if (ms < mejor_makespan_iteracion) {
                mejor_makespan_iteracion = ms;
                mejor_secuencia_iteracion = temp_secuencia;
            }
        }
        secuencia_actual = mejor_secuencia_iteracion;
        mejor_makespan_final = mejor_makespan_iteracion;
    }


    cout << "Mejor secuencia: ";
    for (int t : secuencia_actual) cout << (t + 1) << " ";

    cout << "\nMakespan final: " << mejor_makespan_final << endl;
    return secuencia_actual;
}

vector<int> perturbacion(vector<int>& secuencia,  const vector<vector<int>>& tiempos, int m){
    
    int n = secuencia.size();
    int i = rand() % n;
    int j = rand() % n;
    if (i == j){
        int j = rand() % n;
    };

    vector<int> vecino = secuencia;

    int job = vecino[i];
    vecino.erase(vecino.begin() + i);
    vecino.insert(vecino.begin() + j, job);

    return vecino;
}


vector<int> sa(vector<int>& secuencia_actual, const vector<vector<int>>& tiempos, int m){
    vector<int> mejor_secuencia = secuencia_actual;
    double temp = 0.2;
    double eer = 7.0/N;
    double alpha = 1 - eer;
    
    

    for (int i = 1; i <= N; i++){
        for (int t = 1; t <= N_t; t++){
            vector<int> secuencia_perturbada = perturbacion(secuencia_actual, tiempos, m);
            double delta = calcular_makespan(secuencia_perturbada, tiempos, m) - calcular_makespan(secuencia_actual, tiempos, m);
            double p = exp(-(delta/(boltzmann*temp)));
            
            random_device rd;
            mt19937 gen(rd());
            uniform_real_distribution<double> distrib(0.0, 1.0);
            double random_decimal = distrib(gen);

            
            if (delta < 0 or p >  random_decimal){
                secuencia_actual = secuencia_perturbada;
            }
            if (calcular_makespan(secuencia_perturbada, tiempos, m) - calcular_makespan(mejor_secuencia, tiempos, m) < 0){
                mejor_secuencia = secuencia_perturbada;
            }
        }
        temp = temp * alpha;
        
        if (temp < 0.01){
            break;
        }
        
    }
    float mejor_makespan_final = calcular_makespan(mejor_secuencia, tiempos, m);
    return mejor_secuencia;
}

int main() {
    auto instances = get_taillard_benchmark_instances();

    for (const auto& instance : instances) {

        vector<int> instancia_inicial = neh(instance.n, instance.m, instance.tiempos);
        cout << "Instancia: " << instance.name << " (n=" << instance.n << ", m=" << instance.m << ", seed=" << instance.seed << ")" << endl;
        

        auto inicio_grasp = chrono::high_resolution_clock::now();
        vector<int> secuencia_sa = sa(instancia_inicial, instance.tiempos, instance.m); 
        auto fin_grasp = chrono::high_resolution_clock::now();
        chrono::duration<double> tiempo_grasp = fin_grasp - inicio_grasp;

        cout << "Secuencia SA" << "): ";
        for (int t : secuencia_sa) cout << t+1 << " ";
        cout << "\nMakespan SA: " << calcular_makespan(secuencia_sa, instance.tiempos, instance.m) << endl;
        cout << "Tiempo SA: " << tiempo_grasp.count() << "s" << endl;
        cout << "-------------------" << endl;
        
    }

    return 0;
}
