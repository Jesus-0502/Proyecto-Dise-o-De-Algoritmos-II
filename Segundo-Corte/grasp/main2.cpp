#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include "grasp.h"

using namespace std;

int main() {
    int n = 20, m = 10;

    vector<vector<int>> tiempos = {
    { 3, 18, 19,  2, 16, 18, 19, 23, 25,  1},
    {21, 14,  2,  8,  7, 27,  7,  4, 11, 19},
    { 5, 10,  9, 19, 11, 12, 29, 10,  8, 27},
    {21, 21, 15, 17, 19, 25,  7, 22, 13, 18},
    {25, 16, 12, 12,  8, 14, 29, 14, 12,  9},
    { 6, 25, 10, 15, 20, 15, 28,  6, 25, 22},
    {22,  2,  8, 16, 23, 21, 24, 27, 23,  6},
    {18, 17, 20, 14,  5, 21, 30, 18,  6, 14},
    {27, 24, 10, 11, 28,  2, 24, 12,  5, 27},
    { 1,  3,  8, 11, 11,  9, 22,  7, 22, 16},
    {21,  3, 30, 19, 16,  5, 19,  9, 10, 15},
    {20,  5,  6,  8,  6,  5, 10, 12,  2,  1},
    { 4, 11,  6, 17, 19, 13, 26, 16,  9, 20},
    {16,  6, 17, 13,  5, 17, 30, 25, 13, 16},
    { 2, 16, 28, 21, 13, 21, 10, 30, 23, 11},
    {20, 30, 11, 18,  7, 16, 23,  6, 21, 30},
    {12, 28,  1,  3, 20, 21, 28,  8,  7,  1},
    {13,  2, 17, 30,  5, 17, 16, 10, 17, 14},
    {16, 21,  3,  9, 22, 23, 22,  2, 12, 28},
    {10, 21, 27, 10, 19, 23, 25, 16,  7, 27}
};

    srand(time(NULL));

    cout << "=== COMPARATIVA DE ALGORITMOS FLOWSHOP ===" << endl << endl;

    auto inicio_neh = chrono::high_resolution_clock::now();
    vector<int> secuencia_inicial = neh(n, m, tiempos);
    auto fin_neh = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_neh = fin_neh - inicio_neh;

    cout << "Secuencia NEH: ";
    for (int t : secuencia_inicial) cout << t+1 << " ";
    cout << "\nMakespan NEH: " << calcular_makespan(secuencia_inicial, tiempos, m) << endl;
    cout << "Tiempo NEH: " << tiempo_neh.count() << "s" << endl << "-------------------" << endl;

    auto inicio_ls = chrono::high_resolution_clock::now();
    vector<int> secuencia_ls = local_search_insertion(secuencia_inicial, tiempos, m);
    auto fin_ls = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo_ls = fin_ls - inicio_ls;

    cout << "Secuencia Local Search: ";
    for (int t : secuencia_ls) cout << t+1 << " ";
    cout << "\nMakespan LS: " << calcular_makespan(secuencia_ls, tiempos, m) << endl;
    cout << "Tiempo LS: " << tiempo_ls.count() << "s" << endl << "-------------------" << endl;

    int iteraciones_grasp = 50;
    double alphas[] = {0.1, 0.3, 0.5, 0.7, 0.9};
    for (double alpha : alphas) {

        auto inicio_grasp = chrono::high_resolution_clock::now();
        vector<int> secuencia_grasp = ejecutarGRASP(n, m, tiempos, iteraciones_grasp, alpha);
        auto fin_grasp = chrono::high_resolution_clock::now();
        chrono::duration<double> tiempo_grasp = fin_grasp - inicio_grasp;

        cout << "Secuencia GRASP (Alpha=" << alpha << "): ";
        for (int t : secuencia_grasp) cout << t+1 << " ";
        cout << "\nMakespan GRASP: " << calcular_makespan(secuencia_grasp, tiempos, m) << endl;
        cout << "Tiempo GRASP: " << tiempo_grasp.count() << "s" << endl;
        cout << "-------------------" << endl;
    }

    return 0;
}