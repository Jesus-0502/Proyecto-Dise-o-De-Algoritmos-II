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
    {24, 16, 29, 20, 11, 29, 11, 12,  9, 10},
    {10, 26, 28, 14, 30,  9,  8, 23, 30,  3},
    {25, 13, 14, 22, 27, 30,  6, 19,  1, 21},
    {22, 30,  8,  1, 12, 26,  5, 15, 29, 20},
    { 6, 20,  8,  1, 14, 29, 16,  9, 25, 30},
    {21, 21, 14, 11, 26, 14, 25, 30, 20, 14},
    {24, 22, 11,  9, 28,  9, 22, 17, 13, 20},
    {30, 10, 10, 29, 28, 21,  6,  5,  6, 13},
    { 7, 18,  5, 12, 10, 26,  2,  5,  1,  3},
    {26, 15, 13, 30,  4, 15, 17, 25, 23, 12},
    {29, 27, 14, 27, 23, 25, 28, 16,  3,  1},
    {22, 19, 13, 28, 27, 10, 10, 11, 22, 16},
    { 4, 19,  6,  7, 30, 27,  7,  3, 26,  4},
    {26, 26,  1,  8, 12, 26,  4, 26, 11, 26},
    { 3,  7, 11, 12, 15, 11, 18, 29, 22, 11},
    {16,  5,  3, 28,  5,  4, 29, 15, 16, 27},
    {12, 19, 22, 22,  6, 21, 30, 11, 19, 10},
    { 4, 17,  8, 21, 19, 22, 18, 23,  9, 28},
    { 5, 17, 21,  1,  6,  9, 27, 18, 30, 18},
    {30, 20, 10, 13, 19, 29, 30,  7, 28, 14},
    {12, 14,  7, 18, 13, 16,  5, 29, 13, 18},
    {10,  2, 11, 14, 28,  9, 23, 29, 15, 15},
    {15, 28,  1, 15, 17, 22,  5, 30, 12, 28},
    {12, 14, 28, 18, 19, 22, 20, 18, 10,  3},
    {30, 10,  9, 22, 19, 14,  3, 12,  3, 16},
    {25, 19,  3, 14, 13, 20, 22, 24, 14, 17},
    {19, 29, 17, 27, 24, 22,  2, 23,  8, 24},
    {27, 26,  7, 13, 16, 29, 20,  7, 25,  1},
    {25, 15, 24, 21, 26, 29, 25,  1,  2, 26},
    {26, 18, 23, 25, 15, 16,  6, 11, 22,  6},
    {15,  7, 26, 28, 23, 15, 27, 19, 30, 16},
    { 8, 19, 28,  1, 30, 28, 10, 29,  9, 12},
    { 9, 28,  5,  2, 25,  1, 23,  3, 27, 15},
    {25, 22, 26, 11,  7,  8, 27, 24,  3, 23},
    { 8, 28,  2, 30,  3, 11,  5, 30,  9, 24},
    {17, 22,  5, 27, 13,  5, 23, 23, 29, 16},
    {23, 30,  8, 26,  4,  5, 21, 30, 23, 20},
    { 4, 19, 19, 18, 20, 30, 29, 23,  7,  5},
    { 2,  9,  9, 23, 29,  8, 13, 19, 21,  2},
    {30, 16, 27,  3, 28,  1,  1,  9,  6,  5},
    { 6, 26, 27, 19, 11, 11, 10, 15,  8, 25},
    {19, 21, 15, 25, 21, 11,  2, 18, 28, 15},
    {28, 25, 23, 27, 12, 26,  7, 10,  2,  2},
    { 2, 11,  1, 14,  8, 18,  3,  9, 10, 21},
    {16, 23, 10,  6, 24, 30, 19,  4,  7, 11},
    { 5,  7,  7, 21, 10, 28, 12, 18, 15, 11},
    {15, 19, 11, 13,  8,  6, 18, 19, 27, 23},
    { 9,  8, 11, 12,  5, 17, 18, 15, 21, 29},
    {21,  6, 10, 17,  6,  3, 10, 22,  5, 23},
    {21,  1, 13, 26, 23,  9, 10, 14,  7,  1}
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