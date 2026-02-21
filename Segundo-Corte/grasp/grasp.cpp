#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "LS.h"
#include "NEH.h"

using namespace std;

const int INF = 1e9;

// Función para evaluar el costo de agregar un nuevo trabajo a la secuencia actual
int evaluarCandidato(vector<int> secuenciaActual, int trabajoNuevo, int n, int m, const vector<vector<int>>& tiempos) {
    secuenciaActual.push_back(trabajoNuevo);
    return calcular_makespan(secuenciaActual, tiempos, m);
}

// Función para elegir un candidato de la RCL
int elegirDeRCL(const vector<int>& candidatos, const vector<int>& secuencia, double alpha, int n, int m, const vector<vector<int>>& tiempos) {
    int c_min = INF, c_max = -INF;
    vector<int> costos;

    for(int j : candidatos) {
        int c = evaluarCandidato(secuencia, j, n, m, tiempos);
        costos.push_back(c);
        if(c < c_min) c_min = c;
        if(c > c_max) c_max = c;
    }

    double umbral = c_min + alpha * (c_max - c_min);
    vector<int> rcl_indices;
    for(int i=0; i < candidatos.size(); i++) {
        if(costos[i] <= umbral) rcl_indices.push_back(i);
    }
    
    return rcl_indices[rand() % rcl_indices.size()];
}

// Fase de Construcción
vector<int> faseConstruccion(int n, int m, double alpha, const vector<vector<int>>& tiempos) {
    vector<int> secuencia;
    vector<int> candidatos;
    for(int i=0; i<n; i++) candidatos.push_back(i);

    while(!candidatos.empty()) {
        int index = elegirDeRCL(candidatos, secuencia, alpha, n, m, tiempos);
        secuencia.push_back(candidatos[index]);
        candidatos.erase(candidatos.begin() + index);
    }
    return secuencia;
}


// Algoritmo GRASP Principal
vector<int> ejecutarGRASP(int n, int m, const vector<vector<int>>& tiempos, int maxIter, double alpha) {
    vector<int> mejorGlobal;
    int mejorCostoGlobal = INF;

    for(int i=0; i < maxIter; i++) {
        vector<int> sol = faseConstruccion(n, m, alpha, tiempos);
        sol = local_search_insertion(sol, tiempos, m);
        
        int costo = calcular_makespan(sol, tiempos, m);
        if(costo < mejorCostoGlobal) {
            mejorCostoGlobal = costo;
            mejorGlobal = sol;
        }
    }
    return mejorGlobal;
}