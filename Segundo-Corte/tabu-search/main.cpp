#include <ctime>
#include <iostream>
#include "NEH.h"
#include "TS.h"
#include <chrono>

using namespace std;

int main() {
    int n = 20, m = 5;

    vector<vector<int>> tiempos = {
    {54, 79, 16, 66, 58},
    {83,  3, 89, 58, 56},
    {15, 11, 49, 31, 20},
    {71, 99, 15, 68, 85},
    {77, 56, 89, 78, 53},
    {36, 70, 45, 91, 35},
    {53, 99, 60, 13, 53},
    {38, 60, 23, 59, 41},
    {27,  5, 57, 49, 69},
    {87, 56, 64, 85, 13},
    {76,  3,  7, 85, 86},
    {91, 61,  1,  9, 72},
    {14, 73, 63, 39,  8},
    {29, 75, 41, 41, 49},
    {12, 47, 63, 56, 47},
    {77, 14, 47, 40, 87},
    {32, 21, 26, 54, 58},
    {87, 86, 75, 77, 18},
    {68,  5, 77, 51, 68},
    {94, 77, 40, 31, 28}
};

   // Solución inicial con NEH
    vector<int> secuencia_inicial = neh(n, m, tiempos);

    // Tabu Search
    int max_iters = 10000;
    int tabu_tenure = 7;
    int max_sin_mejora = 1000;

    auto inicio_ts = chrono::high_resolution_clock::now();
    vector<int> secuencia_tabu = tabu_search(
        secuencia_inicial, tiempos, m, max_iters, tabu_tenure, max_sin_mejora
    );
    auto fin_ts = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_ts = fin_ts - inicio_ts;

    cout << "Secuencia TabuSearch: ";
    for (int x : secuencia_tabu) cout << x + 1 << " ";
    cout << "\nMakespan: " << calcular_makespan(secuencia_tabu, tiempos, m) << endl;
    cout << "Tiempo TS: " << tiempo_ts.count() << " segundos" << endl;
    cout << "\n";
    cout << endl;

}