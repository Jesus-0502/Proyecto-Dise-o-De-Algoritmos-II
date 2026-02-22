#ifndef TS_H
#define TS_H

#include <vector>
using namespace std;

vector<int> tabu_search(vector<int> secuencia_inicial,
                                  const vector<vector<int>>& tiempos,
                                  int m,
                                  int max_iters,
                                  int tabu_tenure,
                                  int max_sin_mejora);

#endif