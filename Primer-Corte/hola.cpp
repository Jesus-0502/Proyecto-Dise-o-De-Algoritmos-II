#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <iomanip>
#include <string>
#include <chrono>

using namespace std;

// Función para generar una matriz de tiempos de procesamiento
vector<vector<int>> generarMatrizPFSP(int n_jobs, int n_machines, int min_time = 1, int max_time = 30) {
    // Semilla aleatoria basada en el tiempo actual
    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    default_random_engine generator(seed);
    uniform_int_distribution<int> distribution(min_time, max_time);
    
    // Crear matriz de n_jobs x n_machines
    vector<vector<int>> matriz(n_jobs, vector<int>(n_machines));
    
    for (int i = 0; i < n_jobs; i++) {
        for (int j = 0; j < n_machines; j++) {
            matriz[i][j] = distribution(generator);
        }
    }
    
    return matriz;
}

// Función para guardar la matriz en un archivo con formato C++ vector<vector<int>>
void guardarMatrizCPP(const vector<vector<int>>& matriz, const string& filename, 
                      int n_jobs, int n_machines) {
    ofstream archivo(filename);
    
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        return;
    }
    
    // Escribir encabezado como comentarios
    archivo << "// =========================================================" << endl;
    archivo << "// Instancia de Permutation Flowshop Problem" << endl;
    archivo << "// Archivo generado automáticamente" << endl;
    archivo << "// Trabajos: " << n_jobs << ", Máquinas: " << n_machines << endl;
    archivo << "// Tiempos de procesamiento aleatorios entre " << 1 << " y " << 30 << endl;
    archivo << "// Cada fila es un trabajo, cada columna es una máquina" << endl;
    archivo << "// =========================================================" << endl;
    archivo << endl;
    
    // Escribir la matriz en formato vector<vector<int>>
    archivo << "vector<vector<int>> matriz = {" << endl;
    
    for (size_t i = 0; i < matriz.size(); i++) {
        archivo << "    {";
        for (size_t j = 0; j < matriz[i].size(); j++) {
            archivo << setw(2) << matriz[i][j];
            if (j < matriz[i].size() - 1) {
                archivo << ", ";
            }
        }
        archivo << "}";
        if (i < matriz.size() - 1) {
            archivo << ",";
        }
        archivo << endl;
    }
    
    archivo << "};" << endl;
    
    archivo.close();
    cout << "Matriz guardada en formato C++ en: " << filename << endl;
}

// Función para guardar la matriz en formato original (para compatibilidad)
void guardarMatrizOriginal(const vector<vector<int>>& matriz, const string& filename, 
                           int n_jobs, int n_machines) {
    ofstream archivo(filename);
    
    if (!archivo.is_open()) {
        cerr << "Error al abrir el archivo: " << filename << endl;
        return;
    }
    
    // Escribir encabezado con información de la instancia
    archivo << "# Instancia de Permutation Flowshop Problem" << endl;
    archivo << "# Trabajos: " << n_jobs << ", Máquinas: " << n_machines << endl;
    archivo << "# Tiempos de procesamiento aleatorios entre 1 y 30" << endl;
    archivo << "# Formato: cada fila es un trabajo, cada columna es una máquina" << endl;
    archivo << "# =========================================" << endl;
    
    // Escribir matriz
    for (const auto& fila : matriz) {
        for (size_t j = 0; j < fila.size(); j++) {
            archivo << setw(3) << fila[j];
            if (j < fila.size() - 1) {
                archivo << " ";
            }
        }
        archivo << endl;
    }
    
    archivo.close();
    cout << "Matriz guardada en formato original en: " << filename << endl;
}

// Función para mostrar una matriz en la consola
void mostrarMatriz(const vector<vector<int>>& matriz, const string& nombre) {
    cout << "\n=== " << nombre << " ===" << endl;
    cout << "Trabajos: " << matriz.size() 
         << ", Máquinas: " << (matriz.empty() ? 0 : matriz[0].size()) << endl;
    
    // Mostrar solo una parte si es muy grande
    int max_filas = 5;
    int max_columnas = 5;
    
    for (int i = 0; i < min((int)matriz.size(), max_filas); i++) {
        for (int j = 0; j < min((int)matriz[i].size(), max_columnas); j++) {
            cout << setw(3) << matriz[i][j] << " ";
        }
        if (matriz[i].size() > max_columnas) {
            cout << "...";
        }
        cout << endl;
    }
    if (matriz.size() > max_filas) {
        cout << "... (" << matriz.size() - max_filas << " filas más)" << endl;
    }
}

// Función para mostrar ejemplo del archivo de salida
void mostrarEjemploSalida(const string& filename) {
    cout << "\nEjemplo del formato del archivo " << filename << ":" << endl;
    cout << "==========================================" << endl;
    cout << "// =========================================================" << endl;
    cout << "// Instancia de Permutation Flowshop Problem" << endl;
    cout << "// Archivo generado automáticamente" << endl;
    cout << "// Trabajos: 20, Máquinas: 5" << endl;
    cout << "// Tiempos de procesamiento aleatorios entre 1 y 30" << endl;
    cout << "// Cada fila es un trabajo, cada columna es una máquina" << endl;
    cout << "// =========================================================" << endl;
    cout << endl;
    cout << "vector<vector<int>> matriz = {" << endl;
    cout << "    { 3, 15, 22,  8, 19}," << endl;
    cout << "    {18,  7, 25, 12,  5}," << endl;
    cout << "    { 9, 28, 14,  3, 21}," << endl;
    cout << "    // ... (17 filas más)" << endl;
    cout << "};" << endl;
    cout << "==========================================" << endl;
}

int main() {
    cout << "=========================================" << endl;
    cout << "Generador de Instancias PFSP - Formato C++" << endl;
    cout << "=========================================" << endl;
    
    // Definir las instancias a generar (trabajos x máquinas)
    vector<pair<int, int>> instancias = {
        {20, 5},   // 20 trabajos, 5 máquinas
        {20, 10},  // 20 trabajos, 10 máquinas
        {50, 10},  // 50 trabajos, 10 máquinas
        {100, 10}, // 100 trabajos, 10 máquinas
        {100, 20}  // 100 trabajos, 20 máquinas
    };
    
    // Nombres de archivo para cada instancia (formato C++)
    vector<string> nombres_cpp = {
        "pfsp_20x5.cpp",
        "pfsp_20x10.cpp",
        "pfsp_50x10.cpp",
        "pfsp_100x10.cpp",
        "pfsp_100x20.cpp"
    };
    
    // Nombres de archivo para formato original
    vector<string> nombres_txt = {
        "pfsp_20x5.txt",
        "pfsp_20x10.txt",
        "pfsp_50x10.txt",
        "pfsp_100x10.txt",
        "pfsp_100x20.txt"
    };
    
    // Mostrar ejemplo del formato de salida
    mostrarEjemploSalida("pfsp_20x5.cpp");
    
    // Generar y guardar cada instancia
    for (size_t i = 0; i < instancias.size(); i++) {
        int n_jobs = instancias[i].first;
        int n_machines = instancias[i].second;
        
        cout << "\nGenerando instancia " << (i+1) << "/" << instancias.size() << ": ";
        cout << n_jobs << " trabajos x " << n_machines << " máquinas" << endl;
        
        // Generar matriz
        vector<vector<int>> matriz = generarMatrizPFSP(n_jobs, n_machines);
        
        // Mostrar vista previa
        mostrarMatriz(matriz, "Instancia " + to_string(n_jobs) + "x" + to_string(n_machines));
        
        // Guardar en formato C++
        guardarMatrizCPP(matriz, nombres_cpp[i], n_jobs, n_machines);
        
        // Guardar también en formato original (opcional)
        guardarMatrizOriginal(matriz, nombres_txt[i], n_jobs, n_machines);
        
        // Calcular estadísticas simples
        int suma_total = 0;
        for (const auto& fila : matriz) {
            for (int tiempo : fila) {
                suma_total += tiempo;
            }
        }
        double promedio = suma_total / (double)(n_jobs * n_machines);
        cout << "Tiempo promedio por operación: " << fixed << setprecision(2) << promedio << endl;
    }
    
    cout << "\n=========================================" << endl;
    cout << "Todas las instancias han sido generadas." << endl;
    cout << endl;
    cout << "Archivos generados:" << endl;
    for (size_t i = 0; i < nombres_cpp.size(); i++) {
        cout << "  • " << nombres_cpp[i] << " (formato C++)" << endl;
        cout << "  • " << nombres_txt[i] << " (formato original)" << endl;
    }
    cout << endl;
    cout << "Para usar los archivos C++, simplemente inclúyelos" << endl;
    cout << "o copia el vector<vector<int>> en tu código." << endl;
    cout << "=========================================" << endl;
    
    // Ejemplo de cómo usar los archivos generados
    cout << "\nEjemplo de uso en tu código:" << endl;
    cout << "==============================" << endl;
    cout << "// Para usar la matriz generada:" << endl;
    cout << "#include <vector>" << endl;
    cout << "#include <iostream>" << endl;
    cout << endl;
    cout << "using namespace std;" << endl;
    cout << endl;
    cout << "int main() {" << endl;
    cout << "    // Copia y pega el contenido del archivo .cpp aquí" << endl;
    cout << "    // O incluye el archivo si lo prefieres" << endl;
    cout << "    " << endl;
    cout << "    // Ejemplo:" << endl;
    cout << "    vector<vector<int>> pfsp_matrix = {" << endl;
    cout << "        {12, 25,  8, 19,  3}," << endl;
    cout << "        { 7, 18, 22,  5, 14}," << endl;
    cout << "        // ... resto de la matriz" << endl;
    cout << "    };" << endl;
    cout << "    " << endl;
    cout << "    cout << \"Matriz cargada: \" << pfsp_matrix.size() << \" trabajos, \"" << endl;
    cout << "         << pfsp_matrix[0].size() << \" máquinas\" << endl;" << endl;
    cout << "    return 0;" << endl;
    cout << "}" << endl;
    
    return 0;
}