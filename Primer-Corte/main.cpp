#include <ctime>
#include <iostream>
#include "NEH.h"
#include "LS.h"
#include "ILS.h"
#include <chrono>

using namespace std;

int main() {
    int n = 20, m = 5;

    vector<vector<int>> tiempos = {
    { 2, 20, 28, 27,  3},
    {26,  2,  1, 30, 27},
    { 7, 15, 20,  9, 11},
    {11, 22, 22, 10, 29},
    {10,  7, 12,  8, 10},
    {19, 21,  6,  7, 12},
    { 1,  9, 28, 29, 29},
    {13, 30, 24, 10, 14},
    { 7, 22, 16, 29, 30},
    {14,  6,  1, 30,  3},
    { 9, 29, 27, 27,  6},
    { 6,  6, 28, 19, 24},
    {27,  6, 21, 26, 26},
    {13,  3, 13, 29,  8},
    {25, 24,  5, 28, 10},
    {27, 13, 12, 17,  1},
    {30, 10,  1, 20, 15},
    { 5, 23, 28,  2,  1},
    {18, 23, 29, 18,  2},
    {19, 20, 20, 23, 18}
};
    // NEH con medición de tiempo
    auto inicio_neh = chrono::high_resolution_clock::now();
    vector<int> secuencia_inicial = neh(n, m, tiempos);
    auto fin_neh = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_neh = fin_neh - inicio_neh;

    cout << "Secuencia NEH: ";
    for (int t : secuencia_inicial) cout << t+1 << " ";
    cout << "\nMakespan: " << calcular_makespan(secuencia_inicial, tiempos, m) << endl;
    cout << "Tiempo NEH: " << tiempo_neh.count() << " segundos" << endl;
    cout << "\n";

    // LS con medición de tiempo
    auto inicio_ls = chrono::high_resolution_clock::now();
    vector<int> secuencia_ls = local_search_insertion(secuencia_inicial, tiempos, m);
    auto fin_ls = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_ls = fin_ls - inicio_ls;

    cout << "Secuencia LocalSearch: ";
    for (int t : secuencia_ls) cout << t+1 << " ";
    cout << "\nMakespan: " << calcular_makespan(secuencia_ls, tiempos, m) << endl;
    cout << "Tiempo LS: " << tiempo_ls.count() << " segundos" << endl;
    cout << "\n";

    // ILS con medición de tiempo
    auto inicio_ils = chrono::high_resolution_clock::now();
    vector<int> secuencia_ils = ILS(secuencia_inicial, tiempos, m, 30);
    auto fin_ils = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_ils = fin_ils - inicio_ils;

    cout << "Secuencia IterativeLocalSearch: ";
    for (int t : secuencia_ils) cout << t+1 << " ";
    cout << "\nMakespan: " << calcular_makespan(secuencia_ils, tiempos, m) << endl;
    cout << "Tiempo ILS: " << tiempo_ils.count() << " segundos" << endl;
}