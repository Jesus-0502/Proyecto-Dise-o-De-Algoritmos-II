#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include "NEH.h"

using namespace std;

// Funcion para calcular el makespan (Cmax)
int calcular_makespan(const vector<int>& secuencia, const vector<vector<int>>& tiempos, int n_maquinas) {
    int n_tareas = secuencia.size();
    if (n_tareas == 0) return 0;

    vector<vector<int>> C(n_tareas + 1, vector<int>(n_maquinas + 1, 0));
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

    //Insercion iterativa de tareas
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
    return secuencia_actual;
}