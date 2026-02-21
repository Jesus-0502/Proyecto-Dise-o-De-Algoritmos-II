#include "LS.h"
#include "NEH.h"  // para usar calcular_makespan
#include <iostream>
using namespace std;

vector<int> local_search_insertion(vector<int> secuencia, 
                                   const vector<vector<int>>& tiempos, 
                                   int m) {
    bool mejora = true;
    int n = secuencia.size();

    while (mejora) {
        mejora = false;
        int mejor_makespan = calcular_makespan(secuencia, tiempos, m);
        vector<int> mejor_vecino = secuencia;

        for (int i = 0; i < n; ++i) {
            
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;
                
                vector<int> vecino = secuencia;
                int tarea = vecino[i];
                vecino.erase(vecino.begin() + i);
                vecino.insert(vecino.begin() + j, tarea);
                int ms = calcular_makespan(vecino, tiempos, m);

                if (ms < mejor_makespan) {
                    // cout << "Consegui uno mejor" << endl;
                    mejor_makespan = ms;
                    mejor_vecino = vecino;
                    mejora = true;
                }
            }
        }

        secuencia = mejor_vecino;
    }

    return secuencia;
}
