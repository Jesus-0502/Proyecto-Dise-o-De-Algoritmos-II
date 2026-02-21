#ifndef ILS_H
#define ILS_H

#include <vector>

std::vector<int> perturbacion_swap(std::vector<int> secuencia, int fuerza);
std::vector<int> ILS(const std::vector<int>& solucion_inicial,
                     const std::vector<std::vector<int>>& tiempos,
                     int m,
                     int max_iters);

#endif