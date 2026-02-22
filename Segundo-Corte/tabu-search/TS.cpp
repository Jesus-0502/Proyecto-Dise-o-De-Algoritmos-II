#include "TS.h"
#include "NEH.h"
#include <vector>
#include <queue>
#include <limits>
using namespace std;

struct MovimientoTabu {
    int job;
    int pos_destino;
};

bool es_tabu(const queue<MovimientoTabu>& lista_tabu, int job, int pos_destino) {
    queue<MovimientoTabu> copia = lista_tabu;
    while (!copia.empty()) {
        MovimientoTabu mov = copia.front();
        copia.pop();
        if (mov.job == job && mov.pos_destino == pos_destino) {
            return true;
        }
    }
    return false;
}

vector<int> tabu_search(vector<int> secuencia_inicial,
                                  const vector<vector<int>>& tiempos,
                                  int m,
                                  int max_iters,
                                  int tabu_tenure,
                                  int max_sin_mejora) {

    int n = secuencia_inicial.size();

    vector<int> secuencia_actual = secuencia_inicial;
    vector<int> mejor_global = secuencia_actual;

    int makespan_actual = calcular_makespan(secuencia_actual, tiempos, m);
    int mejor_makespan_global = makespan_actual;

    queue<MovimientoTabu> lista_tabu;

    int iter_sin_mejora = 0;

    for (int iter = 0; iter < max_iters; ++iter) {

        // Criterio de parada por estancamiento
        if (iter_sin_mejora >= max_sin_mejora) {
            break;
        }

        int mejor_makespan_vecino = numeric_limits<int>::max();
        vector<int> mejor_vecino = secuencia_actual;

        int mejor_job = -1;
        int mejor_pos = -1;

        // Exploración completa de vecindad (inserción)
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                if (i == j) continue;

                vector<int> vecino = secuencia_actual;

                int job = vecino[i];
                vecino.erase(vecino.begin() + i);
                vecino.insert(vecino.begin() + j, job);

                int ms = calcular_makespan(vecino, tiempos, m);

                bool tabu = es_tabu(lista_tabu, job, j);

                // Aspiración: permitir si mejora el mejor global
                if (tabu && ms >= mejor_makespan_global) {
                    continue;
                }

                // Best admissible move
                if (ms < mejor_makespan_vecino) {
                    mejor_makespan_vecino = ms;
                    mejor_vecino = vecino;
                    mejor_job = job;
                    mejor_pos = j;
                }
            }
        }

        // Mover a la mejor solución admisible (puede ser peor que la actual)
        secuencia_actual = mejor_vecino;
        makespan_actual = mejor_makespan_vecino;

        // Actualizar mejor global y control de estancamiento
        if (makespan_actual < mejor_makespan_global) {
            mejor_global = secuencia_actual;
            mejor_makespan_global = makespan_actual;
            iter_sin_mejora = 0; // reinicia el contador
        } else {
            iter_sin_mejora++; // no hubo mejora
        }

        // Actualizar lista tabú (FIFO)
        if (mejor_job != -1) {
            lista_tabu.push({mejor_job, mejor_pos});
            if ((int)lista_tabu.size() > tabu_tenure) {
                lista_tabu.pop();
            }
        }
    }

    return mejor_global;
}