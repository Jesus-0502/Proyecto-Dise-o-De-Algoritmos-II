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
    { 1, 30, 26, 27, 13, 11, 11,  3, 14,  5},
    {26, 17,  6,  8,  9,  6, 11, 15, 23,  9},
    {28, 25, 22,  7, 26,  7, 19, 21, 27, 21},
    { 1, 14,  4,  4, 15, 14, 29, 20, 19, 11},
    {17, 28, 22, 17,  1, 14,  7, 29,  7, 26},
    { 1, 11,  4,  3, 13, 11,  7,  6, 22,  5},
    { 3, 24, 13,  9, 15,  3, 12, 18, 23,  8},
    {22, 27,  4, 10, 10, 16,  5, 13,  2, 23},
    { 4, 22, 13,  2, 11, 13, 29, 19, 30,  6},
    {23,  2, 16, 25, 18, 14, 21,  2, 25, 10},
    {18, 18, 19, 16, 13, 29, 30,  2, 13, 25},
    {20,  2,  5,  9, 28, 26, 19, 11, 10, 12},
    {25,  7,  3,  5, 23, 25, 17, 24, 30,  1},
    {19, 28, 19, 21, 10,  8, 17,  1, 20, 22},
    { 3, 28, 28, 14, 20, 21, 23, 25,  5, 29},
    {17,  5, 16, 15,  6,  8,  8, 10, 30, 23},
    {11, 15, 16, 10, 19, 24, 12,  3, 17, 28},
    {22, 13, 11,  9,  5, 16,  8, 21, 30,  3},
    {25, 20, 27, 17,  7, 12, 22, 24,  5, 25},
    {11, 17, 28,  3, 20, 10, 26, 20, 18,  2},
    {11,  3, 24,  1, 25, 13, 18, 25,  1, 25},
    { 6,  7, 19, 28, 21, 17, 23, 20, 29,  1},
    {25, 28, 19,  8, 27, 27, 21, 14,  2, 24},
    { 5,  7, 26,  2,  7, 10, 27, 16, 11,  8},
    {17,  1, 22, 19, 20, 28, 14,  2, 27, 25},
    {30,  1, 19, 27, 24, 22, 15, 28, 11, 25},
    {14, 18, 30,  8, 30, 13, 26,  6,  2,  6},
    {10, 15,  9,  6, 10, 12,  5, 18, 14, 30},
    {15, 30, 11,  2, 18, 22, 19,  3, 29, 16},
    {13, 16, 20, 11,  1, 10, 22, 22,  9, 10},
    {13, 28,  4,  1,  3, 13,  8, 21,  5,  3},
    {10, 16, 10, 24,  7, 13, 30, 13, 21,  8},
    {10, 22, 27, 23, 15, 14, 19,  3, 12,  3},
    { 4,  9, 26,  7, 28, 28,  3, 14,  5, 24},
    {27,  2, 13, 19,  9, 11, 27, 11,  3,  8},
    {21, 21, 11, 26,  4, 17,  8,  9, 24,  5},
    { 6, 28, 18,  1, 28,  6, 13, 19, 20, 25},
    {11, 26, 21, 19, 20,  6, 25, 16, 17, 30},
    {14, 30,  2,  8, 30, 28, 17,  8, 21, 22},
    {19, 15, 25, 26, 30, 13, 12, 15, 10, 13},
    { 5, 11, 13, 19,  5, 12,  1, 16, 23, 21},
    { 2,  8, 20, 15, 22, 12, 30, 23, 29,  9},
    {16,  8, 11,  6, 19, 21,  6, 18, 26, 26},
    {15,  2,  8, 28, 12, 19, 14,  4, 19, 16},
    {26, 20, 22,  5,  2, 17,  6, 29,  5, 10},
    { 2, 20, 17, 19, 20,  6, 13, 29, 19, 16},
    { 9, 18,  7, 24, 20, 27,  9, 29, 24, 19},
    { 3, 24,  2, 27, 28, 14, 26, 14,  7,  7},
    {18,  2,  6, 29, 19,  1, 22, 12,  3,  7},
    {13, 23,  6, 20, 24, 19,  1, 13,  7,  5},
    { 2, 24, 13, 17,  8, 11,  1, 15, 12, 30},
    {22, 28, 17, 22,  5,  4, 19, 25, 26, 17},
    {10,  7,  5, 13, 14, 22,  5, 15,  1, 10},
    { 2, 21, 23, 26, 14,  3, 23, 10,  4, 20},
    {18, 18, 21, 13, 12, 20, 30, 16, 10, 13},
    {14, 24, 19, 17,  8,  8, 29, 26,  3, 15},
    { 8, 19,  1,  4,  1,  2, 22, 30, 29, 10},
    {14,  8, 28,  6, 17,  6, 22, 25,  7, 21},
    {10, 22,  4, 23, 14, 11,  5, 11, 11,  3},
    {27, 13, 27, 19, 10, 16, 11,  6, 13,  6},
    { 3, 29,  6,  9, 16, 25,  2,  7,  8, 11},
    {28, 22, 30, 20,  1, 23, 13, 16,  3, 26},
    { 1,  7, 15, 22, 22, 29,  3, 13,  4, 11},
    { 4, 13, 30, 21, 14,  6, 16, 13, 22, 18},
    { 5, 10,  3, 24,  3, 13, 16, 22, 28,  6},
    {25, 29, 18,  6,  4, 20, 26, 26, 21, 15},
    { 4,  6, 24, 29, 14, 29,  8,  8, 19, 11},
    { 2, 29, 10,  3, 18,  9, 26,  2, 13, 11},
    {17, 27,  3, 24, 28, 10, 13, 20, 22,  4},
    { 9,  8, 20,  1, 11, 27,  6, 21, 26,  8},
    { 7,  5,  3,  1, 30,  8, 17,  6, 10, 30},
    {28, 10, 25, 11, 19, 30,  6, 30, 11,  8},
    {12, 17, 30, 18, 13, 30, 20, 15, 17, 28},
    {13, 10, 11,  8, 29, 27, 27, 16, 17, 14},
    {16,  6, 10, 18, 16,  3, 26,  1, 25,  1},
    { 3, 22,  7, 28, 20, 21, 17, 26, 29,  7},
    {21, 15, 15,  7,  5, 20, 19, 30, 11, 21},
    { 7, 19, 23,  7, 10, 13, 17, 28,  6, 15},
    {20,  5,  6, 17, 16, 27, 23, 10, 26,  3},
    {26, 19, 16,  9, 13, 26, 21, 19, 19, 20},
    {17, 18,  4, 28, 28,  9,  9,  8,  2, 18},
    {20, 12, 23, 23,  2,  4, 18, 15, 18, 27},
    {18, 26, 24,  1,  8, 19,  2, 27,  3, 23},
    {29, 25,  7, 18, 29, 19, 17, 10, 13,  9},
    {21, 25,  9,  9, 27,  5, 30, 30, 19,  8},
    { 7, 11, 16,  2,  3,  7,  8,  4, 19, 23},
    {26, 24, 24,  9,  5, 16,  3,  7,  3,  7},
    {16, 12, 24,  8,  6,  9, 21, 17,  6, 17},
    {21, 21, 27,  1, 11, 17, 24, 12, 11, 15},
    {10, 27, 26, 24,  1, 11, 30, 25, 16, 26},
    {23, 29, 30, 14, 21,  1,  7,  5,  8, 11},
    {30, 19,  3,  9,  8, 27, 20, 23,  6, 20},
    { 1,  7, 10, 29, 12, 25, 29, 14, 26,  1},
    {23, 26,  2,  7, 20, 29, 25, 17,  4, 30},
    {10, 27,  6,  5, 20, 26, 18,  3, 27, 11},
    { 6,  2, 19,  5, 28, 23, 14,  3,  2, 15},
    {25, 15, 30, 21,  4,  1, 25, 20, 21,  2},
    { 5, 14, 23, 20, 28, 22,  3, 23, 20, 13},
    { 6,  7, 16, 30, 12, 24, 24,  6, 21, 17},
    {18, 19, 30, 11, 29, 28, 13, 19, 19, 15}
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