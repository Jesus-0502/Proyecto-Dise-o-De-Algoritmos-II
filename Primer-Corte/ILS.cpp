#include "ILS.h"
#include "LS.h"
#include "NEH.h"
#include <cstdlib>
#include <ctime>
#include <iostream>

using namespace std;


vector<int> perturbacion_swap(vector<int> secuencia, int fuerza) {
    int n = secuencia.size();

    for (int k = 0; k < fuerza; ++k) {
        int i = rand() % n;
        int j = rand() % n;
        swap(secuencia[i], secuencia[j]);
    }
    return secuencia;
}

vector<int> ILS(const vector<int>& solucion_inicial,
                const vector<vector<int>>& tiempos,
                int m,
                int max_iters) {

    vector<int> actual = local_search_insertion(solucion_inicial, tiempos, m);
    vector<int> mejor = actual;

    int mejor_coste = calcular_makespan(mejor, tiempos, m);

    for (int iter = 0; iter < max_iters; ++iter) {

        // Perturbacion
        vector<int> perturbada = perturbacion_swap(actual, 5);

        // Busqueda local
        vector<int> refinada = local_search_insertion(perturbada, tiempos, m);
        int coste = calcular_makespan(refinada, tiempos, m);

        // Aceptacion
        if (coste < mejor_coste) {
            mejor = refinada;
            mejor_coste = coste;
            break;
        }
        actual = refinada;  
    }
    return mejor;
}
