#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>
#include <chrono>

using namespace std;

// Función para calcular la matriz de tiempos de finalización para una secuencia parcial
vector<vector<int>> calculate_partial_completion(const vector<int>& partial_seq, 
                                                 const vector<vector<int>>& processing_times) {
    /*
    Calcula la matriz de tiempos de finalización para una secuencia parcial de trabajos
    en un Permutation Flow Shop.
    
    Args:
        partial_seq: Índices de trabajos ya secuenciados
        processing_times: Matriz MxN de tiempos de procesamiento (M máquinas x N trabajos)
        
    Returns:
        Matriz de tiempos de finalización de tamaño M x len(partial_seq)
    */
    
    if (partial_seq.empty()) {
        return vector<vector<int>>(processing_times.size(), vector<int>());
    }
    
    int num_machines = processing_times.size();
    int num_jobs = partial_seq.size();
    vector<vector<int>> C(num_machines, vector<int>(num_jobs, 0));
    
    for (int j = 0; j < num_jobs; j++) {
        int job = partial_seq[j];
        for (int m = 0; m < num_machines; m++) {
            if (j == 0 && m == 0) {
                C[m][j] = processing_times[m][job];
            } else if (j == 0) {
                C[m][j] = C[m-1][j] + processing_times[m][job];
            } else if (m == 0) {
                C[m][j] = C[m][j-1] + processing_times[m][job];
            } else {
                C[m][j] = max(C[m-1][j], C[m][j-1]) + processing_times[m][job];
            }
        }
    }
    
    return C;
}

// Función para calcular la cota inferior en Branch and Bound
int lower_bound_bb(const vector<vector<int>>& C_partial,
                   const vector<int>& remaining_jobs,
                   const vector<vector<int>>& processing_times) {
    /*
    Calcula una cota inferior realista para una secuencia parcial en Branch and Bound.
    
    La cota inferior se calcula como el máximo entre los tiempos de finalización actuales
    de la secuencia parcial y una estimación optimista de los trabajos restantes.
    */
    
    if (remaining_jobs.empty()) {
        if (C_partial.empty() || C_partial[0].empty()) {
            return 0;
        }
        return C_partial.back().back(); // Último elemento de la última máquina
    }
    
    int num_machines = processing_times.size();
    
    // Últimos tiempos de finalización de la secuencia parcial
    vector<int> last_times(num_machines, 0);
    if (!C_partial.empty() && !C_partial[0].empty()) {
        for (int m = 0; m < num_machines; m++) {
            last_times[m] = C_partial[m].back();
        }
    }
    
    // Estimación optimista: suma de tiempos de procesamiento de trabajos restantes por máquina
    vector<int> est_remaining(num_machines, 0);
    for (int m = 0; m < num_machines; m++) {
        for (int job : remaining_jobs) {
            est_remaining[m] += processing_times[m][job];
        }
    }
    
    // Calcular el máximo considerando las restricciones de las máquinas
    int bound = 0;
    for (int m = 0; m < num_machines; m++) {
        int machine_bound = last_times[m] + est_remaining[m];
        if (machine_bound > bound) {
            bound = machine_bound;
        }
    }
    
    return bound;
}

// Variables globales para la recursión
int best_flowtime;
vector<int> best_sequence;

// Función recursiva de exploración en Branch and Bound
void explore(const vector<int>& partial_seq,
             const vector<int>& remaining_jobs,
             const vector<vector<int>>& C_partial,
             const vector<vector<int>>& processing_times) {
    
    // Calcular cota inferior
    int lb = lower_bound_bb(C_partial, remaining_jobs, processing_times);
    
    // Podar si la cota es mayor o igual al mejor flujo encontrado
    if (lb >= best_flowtime) {
        return;
    }
    
    // Si no quedan trabajos, hemos encontrado una secuencia completa
    if (remaining_jobs.empty()) {
        if (!C_partial.empty() && !C_partial[0].empty()) {
            int current_flowtime = C_partial.back().back();
            if (current_flowtime < best_flowtime) {
                best_flowtime = current_flowtime;
                best_sequence = partial_seq;
            }
        }
        return;
    }
    
    // Explorar todos los trabajos restantes
    for (size_t i = 0; i < remaining_jobs.size(); i++) {
        // Crear nueva secuencia parcial
        vector<int> new_seq = partial_seq;
        new_seq.push_back(remaining_jobs[i]);
        
        // Crear nueva lista de trabajos restantes
        vector<int> new_remaining;
        for (size_t j = 0; j < remaining_jobs.size(); j++) {
            if (j != i) {
                new_remaining.push_back(remaining_jobs[j]);
            }
        }
        
        // Calcular nueva matriz de finalización
        vector<vector<int>> C_new = calculate_partial_completion(new_seq, processing_times);
        
        // Llamada recursiva
        explore(new_seq, new_remaining, C_new, processing_times);
    }
}

// Función principal de Branch and Bound
pair<vector<int>, int> branch_and_bound(const vector<vector<int>>& processing_times) {
    /*
    Resuelve el problema PFSP usando Branch and Bound exacto.
    
    Args:
        processing_times: Matriz MxN de tiempos de procesamiento
        
    Returns:
        pair<mejor_secuencia, mejor_flowtime>
    */
    
    int num_jobs = processing_times[0].size();
    
    // Inicializar variables globales
    best_flowtime = numeric_limits<int>::max();
    best_sequence.clear();
    
    // Crear lista de todos los trabajos
    vector<int> all_jobs;
    for (int i = 0; i < num_jobs; i++) {
        all_jobs.push_back(i);
    }
    
    // Iniciar exploración
    vector<int> empty_seq;
    vector<vector<int>> empty_C(processing_times.size(), vector<int>());
    explore(empty_seq, all_jobs, empty_C, processing_times);
    
    return make_pair(best_sequence, best_flowtime);
}

// Función para transponer una matriz
vector<vector<int>> transpose_matrix(const vector<vector<int>>& matrix) {
    if (matrix.empty()) return {};
    
    int num_machines = matrix[0].size();
    int num_jobs = matrix.size();
    
    vector<vector<int>> transposed(num_machines, vector<int>(num_jobs, 0));
    
    for (int i = 0; i < num_jobs; i++) {
        for (int j = 0; j < num_machines; j++) {
            transposed[j][i] = matrix[i][j];
        }
    }
    
    return transposed;
}

// Función para medir tiempo de ejecución
void measure_execution_time() {
    // Datos de ejemplo (matriz 20x5)
    vector<vector<int>> processing_times = {
    {23, 24,  9,  9, 17,  8, 30,  5, 13, 11, 12,  5, 18,  8, 14, 16,  4, 13,  3, 29},
    {29, 19, 25, 24, 11, 15, 26,  3, 12, 28, 10, 20, 14, 16, 19,  5,  3, 24, 14, 19},
    { 9,  1, 21, 12, 18,  4, 19, 26,  2, 23,  8,  1,  9, 22, 20,  9,  3, 24, 14, 10},
    {21, 15, 18, 17, 23, 15, 28,  6, 16, 16,  3, 10, 11,  1,  6, 28, 12,  3, 30,  8},
    {24, 25,  4, 25, 17, 24,  9, 21, 10,  7, 28, 15, 14, 21,  8, 29, 22,  7,  1, 10},
    {24,  9, 29, 30, 17, 12, 14, 11, 16,  5, 28, 18, 19,  2,  2, 23, 30,  6, 28, 29},
    { 1,  7,  3, 14,  8, 18, 17,  6, 23, 19, 23, 28, 25, 18, 21, 13,  1, 19, 25, 27},
    {11,  7, 20, 29, 29, 19,  6, 22, 27, 29, 29, 11, 25, 10, 13, 26, 12, 25, 23, 17},
    {12, 14, 30, 15,  3, 15,  3,  5, 19, 15, 13,  8, 23, 30,  8,  1, 21,  7, 27, 30},
    {15,  1, 17, 10, 25, 12, 17, 16, 17, 30, 24, 13, 22, 29,  3, 16, 14,  9, 23, 30},
    {25, 13,  9, 17, 16, 10, 12, 13, 11,  3,  3, 10, 27, 17, 11, 21, 17, 12, 12, 25},
    {29, 23,  7, 19,  6, 29, 29,  7, 16, 25,  7, 17,  1,  1, 10, 25,  9,  3, 22, 21},
    {16,  3, 11, 11, 23, 24, 17, 11, 10,  3, 20, 10, 16,  8, 15, 14,  9, 25, 10,  2},
    {25, 18, 30, 28,  9, 29, 29, 22, 20, 29, 23, 23, 25, 13, 22,  8, 26, 25, 23,  3},
    {28, 22,  4, 14, 12, 28, 14, 10,  9,  6, 28,  2, 17, 22, 18, 24, 14, 12, 19, 15},
    { 3, 20, 14, 14, 25, 11, 24, 24, 20,  2, 24, 27, 10, 26, 16, 27,  3, 12, 30,  5},
    {24,  9, 23,  8, 22, 17, 24, 26, 13,  8, 11, 22, 24,  2,  6,  3, 25,  5, 27, 30},
    {10, 25, 25, 20, 10, 30, 20, 26, 24, 29, 20, 26, 13, 12,  3, 10, 24, 25,  5, 17},
    {30, 10, 27,  1, 22, 26, 10, 15, 19, 22, 23, 10, 14, 13,  6, 15, 24, 20, 12,  4},
    {26, 30, 30, 14, 19,  2,  5, 14, 24,  6,  3, 25, 11, 27, 28,  4, 18, 19, 30, 15},
    { 5, 20,  4, 10, 23,  2,  3, 13, 28, 11, 26,  6,  9, 16, 25, 27, 29,  6, 24, 25},
    {19, 24,  7, 18, 29, 21, 27,  3, 19,  3, 17,  5, 10, 23, 18, 12, 21,  3, 27, 25},
    { 6, 18, 11, 14, 27, 28, 27,  7, 16, 26, 12, 22, 12, 19, 16, 27,  2, 24, 14, 24},
    { 5,  4, 16, 17, 12, 28,  4,  7, 13, 26, 22,  7, 17, 28, 15, 19,  9,  5, 12, 15},
    { 7,  1,  9, 24, 29, 26, 17, 22, 23, 19, 13, 27, 23, 22, 10, 26, 25,  5,  1, 18},
    {15, 11, 10, 28,  7, 28,  1, 28,  1, 23, 21, 18, 29, 26,  9,  4,  9,  2, 29, 13},
    { 1, 26,  3, 28, 27,  1, 14, 28,  3, 15,  1, 14, 12, 25, 28, 30, 19, 23,  6, 24},
    { 5,  7, 12, 30, 13, 26, 11,  7, 25, 24,  8,  4, 19, 13, 11,  4,  9, 25,  2,  2},
    {23, 11, 30,  6,  8, 22,  7, 20,  1, 17, 16, 19, 17, 12, 23,  3, 22,  8, 25,  7},
    {17, 18,  9, 30, 13,  1, 19, 22, 21,  5, 29, 19, 25,  7, 23, 21, 19,  6, 20,  9},
    {25,  7, 18,  6,  3,  1, 27, 24, 23,  5, 23, 19, 25,  9, 15,  3, 18, 23, 16,  6},
    { 6, 20, 27, 26, 24,  1, 26, 30,  9, 24,  9, 23, 16, 12,  9, 24, 16, 20, 16, 13},
    {22, 22, 27, 25, 15, 29, 25,  9,  1, 14, 21, 27, 28,  9, 26,  4, 23, 26, 17, 25},
    {20,  3, 18, 29, 28, 13,  9, 11, 20,  9,  1,  3, 13, 18, 14, 15,  4, 13, 22, 17},
    {28, 18,  5,  3,  2, 13,  9,  8, 15, 16, 28,  2, 15, 22,  7,  4, 10,  9, 26, 11},
    { 2, 15, 23, 25, 16, 27, 23,  3, 24, 23, 30, 26, 24, 28,  2, 25,  8, 20, 22, 15},
    { 7, 17,  6, 13, 10, 24, 24,  4, 29, 22, 22, 23, 16, 30,  4, 10, 16,  5, 27, 26},
    { 7,  9, 25, 29, 13,  8,  4, 24, 27,  5, 22,  8, 17, 15,  4,  3, 20, 27,  6, 17},
    {25, 23, 10, 24, 24, 27, 17, 26, 20, 30, 18, 21, 11, 29, 25, 25,  3, 10,  7, 30},
    {17,  2, 29, 24, 14, 12, 23,  8, 29,  8, 17, 24, 15,  8, 27,  6, 19, 28, 25, 21},
    { 3, 16, 25, 15, 24,  7, 24, 13, 19, 25,  7, 22, 15, 15, 16, 21, 10, 11, 25,  8},
    { 1, 14,  5, 26,  6, 25,  7, 12, 21, 28, 27, 28, 24, 24, 17,  8, 14, 23, 18, 23},
    { 8,  8, 19, 30,  7, 10,  8, 28, 15,  3,  6, 16, 12, 28, 25, 17, 17, 17, 26, 20},
    {24,  6, 24, 17,  9, 18,  7,  1, 18, 18,  7, 15, 23, 30, 28,  5, 20, 25, 30, 27},
    {27, 21, 29, 24,  9, 19, 22, 19, 13, 15, 24, 29,  2, 18,  4, 21, 27, 24,  5,  4},
    {26, 30, 15,  3, 27, 30, 27, 25, 16, 23, 19, 17, 16,  7, 12, 27,  5, 22,  6,  2},
    {19, 12,  1, 10,  5,  1,  8, 21, 19, 29, 25,  4, 22, 18,  5, 20, 14, 20,  2, 14},
    {11, 27,  7, 15, 29, 18, 18,  2, 27, 29, 27,  2, 17, 30, 14, 23, 26,  7, 23, 10},
    {16,  5,  6,  8,  9, 25, 28, 25, 15, 14, 14, 26, 28, 17, 21,  3, 20, 21, 22, 11},
    {25,  1,  3,  7,  8, 12, 23,  2, 16,  6, 29,  1, 22,  7, 20, 17,  7, 28,  4,  6},
    { 8, 17,  1, 16, 13,  3, 30,  1,  3, 18,  3, 12, 19,  2, 15,  6, 25,  3,  3,  5},
    { 2, 30,  8, 14, 23, 25, 16, 25,  6,  1,  7, 22, 12, 17, 20,  3, 10,  6, 29, 23},
    { 9, 13,  8,  1, 17, 27, 12, 28, 21, 21,  4, 13, 14, 10,  4, 30, 14,  2, 17, 14},
    {10, 26, 25,  9, 15,  5, 29, 11, 24,  2, 28, 29, 16,  6, 11,  9, 17, 12, 15, 17},
    { 2, 30, 28, 27, 15,  8, 27, 14, 22,  5, 29,  6, 22, 18, 30,  8, 12, 29, 23,  7},
    {21,  4, 15, 12, 26,  8, 22, 21,  4, 16,  9, 20,  2, 19, 29,  8, 15, 24, 24, 17},
    {14,  3, 22, 18,  2, 10,  5, 11, 10,  2, 23, 21,  3,  6, 10,  7, 13, 25, 17, 27},
    {28, 11, 20, 20, 13, 27, 20,  3,  3, 13, 27,  6, 27, 13,  9, 12, 16, 10, 10, 14},
    {17, 15,  1,  7, 17,  1,  4,  1,  9, 19, 29,  8, 19,  7,  7,  7,  9, 29, 13,  2},
    {10, 27,  3, 24, 28,  3, 27,  1,  8, 23, 10, 19,  5, 20, 21, 23, 28, 26,  6, 18},
    {17,  5, 19, 17,  1,  6, 12, 16, 29, 27, 20, 22, 19, 12, 30,  6, 23, 28, 29, 21},
    {11,  1, 28, 26, 21, 28, 17, 27, 10, 16, 14, 13,  4, 15,  5, 18,  5, 18, 18, 19},
    { 8,  5, 15,  5, 10, 14, 24, 28,  7, 14, 20, 22,  6, 29, 17, 18, 16, 18,  8, 26},
    { 2,  7,  7, 11,  7, 28, 24,  1, 14,  9,  8, 11, 11, 29, 28, 22, 20, 20, 19,  3},
    {23, 17, 28,  6,  2,  2,  8,  8, 11, 29, 12, 29, 20, 17,  1,  4, 22, 21, 27, 18},
    {15, 30,  1, 22,  1, 20,  6, 24, 30, 17, 11,  9, 19, 18, 15, 17, 22, 10, 13, 29},
    {18,  8, 24,  7,  9,  6,  6, 28, 19,  4, 22,  7, 28, 26, 21, 28, 13, 15,  5, 26},
    { 5, 28,  2, 26, 26,  9, 20, 28, 28, 13, 29, 26, 26, 12,  8,  2, 13, 29, 17,  2},
    {19, 11, 22,  1,  5, 27,  7, 14, 13, 17, 19, 21,  1,  7,  9, 13, 13, 22, 16, 26},
    {29, 23, 30, 21, 13,  2,  7, 22, 21, 27, 22,  5, 24, 27, 12, 13, 11, 28, 17, 19},
    {10, 10,  4,  7,  2, 20, 15, 29, 10, 26, 24, 18, 12, 25,  3, 29,  4,  6, 19, 20},
    {26, 17, 28,  9, 23,  5, 21,  1,  3, 15, 27, 19, 19, 11, 16,  9, 28, 26, 28, 18},
    { 3, 29, 10, 16,  7,  8, 14, 21, 22, 20, 23,  6, 19, 25, 18,  3,  2,  2,  4, 23},
    {22, 30, 26, 25,  9, 17, 17, 29,  7, 17, 17, 16, 14, 11, 27, 18, 28, 12, 24,  5},
    { 5, 20,  1, 28,  8, 16, 16, 30, 19,  4,  7, 22, 16, 12, 17,  3, 18, 26,  6, 26},
    {29, 22,  3,  3, 10, 24, 29, 24,  8, 23, 30, 18, 17, 16,  7, 12,  4,  2,  3, 10},
    { 4, 16,  4, 22, 14, 19, 17, 29, 14,  8, 10,  8, 14, 30,  7, 26, 12, 28, 18,  8},
    {22,  1, 30, 21, 13, 27, 28, 21, 27,  3,  1, 13,  1,  3, 16, 16, 20,  9, 10, 23},
    {10, 27, 20, 22,  6, 26, 21, 11, 17, 12, 27, 21, 22, 15, 24, 28, 15, 29, 18, 21},
    { 2, 27, 17, 26,  6, 25,  4,  7, 29,  9, 27, 25, 23, 13, 28, 29, 24, 11, 18, 16},
    {19, 27, 12, 23, 21, 24, 19, 12,  6, 13,  9, 17, 27, 12, 19,  7,  7,  6,  2, 23},
    {10,  4, 26, 27, 16, 30, 30, 21, 19, 18,  4, 27,  3, 23,  9, 22, 25, 26, 19, 15},
    {26, 28,  3, 28,  7, 10, 19, 14, 15,  9, 10, 15, 17, 28,  3, 16, 18, 17,  9,  3},
    {29, 10, 22, 18, 23, 13, 26,  4, 23, 30, 15, 13, 28, 25,  7,  9, 27, 25, 13, 19},
    { 5, 15,  4, 15, 30, 25, 25, 14,  3, 28, 11,  3,  7, 11, 12, 11, 13, 30,  2,  9},
    {25, 23, 13,  6, 11, 27, 12, 21, 19,  6, 23, 30,  9, 29, 25,  7,  4, 24,  7, 21},
    { 2,  4, 13, 26,  1, 17,  9,  1, 13, 21, 15, 25, 21, 23, 26,  2, 28,  5, 25, 19},
    { 3, 19, 29, 17, 22, 23,  9, 10,  9,  3, 10,  2, 13, 23, 29, 11, 10,  6, 18,  4},
    {11,  8, 28, 30, 22,  4, 26,  8, 14, 13, 21, 22, 27, 13, 20, 18,  2, 25, 27,  8},
    {21, 17,  5, 20,  3, 22,  2, 13, 12, 28, 13,  6, 27, 20,  5,  1,  5, 12, 13, 15},
    { 4, 19, 19,  9,  5, 15, 22, 14,  2, 26, 12, 16, 27, 27, 15, 16, 24, 24,  3, 20},
    {11,  8,  2, 22, 16, 30,  8, 13,  2, 29, 13, 23, 26, 13,  1, 21, 21, 22, 24, 24},
    {30,  1, 23, 25, 16, 18, 25, 29, 19, 20,  4, 21, 14, 11, 21, 10, 11, 16, 26,  4},
    {23,  7, 18, 12,  1, 23, 16, 21, 11, 14, 19, 20, 20, 13, 22,  7, 29,  4, 13, 23},
    { 7, 16, 30,  8, 30,  7, 17, 11,  6, 25, 14, 24, 27, 28, 26,  2,  6, 24,  9, 12},
    {13,  7, 21, 28, 15, 17, 21, 17, 30,  3, 14, 22, 12, 21,  1,  5,  4, 16, 25, 14},
    {15,  5, 26, 15, 25,  9,  5, 19, 20,  9,  3, 30, 23, 24, 21, 13, 19, 28, 15, 19},
    { 1, 23, 14,  5,  9, 11,  1, 26, 19,  9, 23, 20, 10,  7, 10,  6,  3,  5,  9, 18},
    {19,  7, 12, 14,  3, 12, 19, 26,  9, 22, 12, 26,  8, 25,  2, 18,  6, 19, 11, 14},
    { 4, 24, 30,  3,  5, 29, 21,  4,  5,  1, 10, 29,  3, 19, 29,  9, 18,  7,  7, 19}
};
    
    // Transponer la matriz (convertir a formato máquinas x trabajos)
    vector<vector<int>> transposed = transpose_matrix(processing_times);
    
    cout << "Matriz original: " << processing_times.size() 
         << " trabajos x " << processing_times[0].size() << " máquinas" << endl;
    cout << "Matriz transpuesta: " << transposed.size() 
         << " máquinas x " << transposed[0].size() << " trabajos" << endl;
    
    // Medir tiempo de ejecución
    auto start = chrono::high_resolution_clock::now();
    
    // Ejecutar Branch and Bound
    auto result = branch_and_bound(transposed);
    
    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::milliseconds>(end - start);
    
    // Mostrar resultados
    cout << "\n=== RESULTADOS ===" << endl;
    cout << "Mejor secuencia encontrada: ";
    for (int job : result.first) {
        cout << job << " ";
    }
    cout << endl;
    
    cout << "Mínimo flowtime (makespan): " << result.second << endl;
    cout << "Tiempo de ejecución: " << duration.count() << " ms" << endl;
    
    // Verificar secuencia con cálculo directo
    cout << "\n=== VERIFICACIÓN ===" << endl;
    vector<int> verification_seq = result.first;
    vector<vector<int>> C_verif = calculate_partial_completion(verification_seq, transposed);
    int calculated_flowtime = C_verif.back().back();
    cout << "Flowtime calculado para la secuencia: " << calculated_flowtime << endl;
    
    if (calculated_flowtime == result.second) {
        cout << "✓ Verificación exitosa" << endl;
    } else {
        cout << "✗ Error en la verificación" << endl;
    }
}

int main() {
    cout << "==========================================" << endl;
    cout << "ALGORITMO BRANCH AND BOUND PARA PFSP" << endl;
    cout << "==========================================" << endl;
    
    measure_execution_time();
    
    cout << "\n==========================================" << endl;
    cout << "Nota: Para instancias más grandes (n > 10)," << endl;
    cout << "el algoritmo puede ser muy lento debido a" << endl;
    cout << "la complejidad exponencial del problema." << endl;
    cout << "==========================================" << endl;
    
    return 0;
}