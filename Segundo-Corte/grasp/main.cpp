#include <iostream>
#include <vector>
#include <chrono>
#include <ctime>
#include "grasp.h"

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