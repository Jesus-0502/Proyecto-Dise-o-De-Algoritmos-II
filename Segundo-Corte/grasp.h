#ifndef GRASP_H
#define GRASP_H

#include <vector>

    // Declaración de funciones para que el main las reconozca
std::vector<int> neh(int n, int m, const std::vector<std::vector<int>>& tiempos);
int calcular_makespan(const std::vector<int>& secuencia, const std::vector<std::vector<int>>& tiempos, int m);
std::vector<int> local_search_insertion(std::vector<int> secuencia, const std::vector<std::vector<int>>& tiempos, int m);
std::vector<int> ejecutarGRASP(int n, int m, const std::vector<std::vector<int>>& tiempos, int maxIter, double alpha);
#endif