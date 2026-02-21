#ifndef NEH_H
#define NEH_H

#include <vector>

std::vector<int> neh(int n, int m, const std::vector<std::vector<int>>& tiempos);
int calcular_makespan(const std::vector<int>& secuencia, const std::vector<std::vector<int>>& tiempos, int n_maquinas);

#endif