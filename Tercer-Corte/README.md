# Tercer Corte — PFSP

Esta implementación cubre:

- **Pregunta 1**: Algoritmo Memético (extensión del GA).
  - Recombinación de **3 padres**.
  - Mejora local por inserción luego de la recombinación.
- **Pregunta 3**: Optimización por Colonia de Hormigas (ACO).
  - Feromona por par **(posición, trabajo)**.
  - Heurística incremental basada en makespan parcial.
- **Pregunta 4 (parcial)**: ambos algoritmos se ejecutan sobre el mismo benchmark del segundo corte y usando las mismas semillas de Taillard.

> **No se implementa aún la Pregunta 2 (búsqueda dispersa)**, tal como fue solicitado.

## Benchmark

Se reutiliza exactamente `get_taillard_benchmark_instances()` desde:

- `../../Segundo-Corte/genetic-algorithm/Instances.h`

Esto conserva las 5 instancias:

- `pfsp_20x5`
- `pfsp_20x10`
- `pfsp_50x10`
- `pfsp_100x10`
- `pfsp_100x20`

con sus semillas originales de Taillard.

## Compilación y ejecución

### Memético

En `Tercer-Corte/memetic`:

- `make`
- `./ma_pfsp`

### ACO

En `Tercer-Corte/ant-colony`:

- `make`
- `./aco_pfsp`
