# Segundo Corte — Algoritmo Genético para PFSP

## Modelo (según Arık, 2022)

### Fenotipo y genotipo
- **Genotipo**: una **permutación** de $n$ trabajos, representada como un vector de índices de trabajos (codificación por permutación).
- **Fenotipo**: el **schedule** en flowshop que ejecuta los trabajos en el mismo orden sobre todas las máquinas; su calidad se mide con el **makespan $C_{max}$**.

### Función objetivo y evaluación
- Se minimiza $C_{max}$.
- Para selección proporcional se usa **fitness** creciente con mejor calidad: $f = 1/(1 + C_{max})$.

### Operadores del GA
- **Selección**: **ruleta (roulette wheel)** con probabilidades proporcionales al fitness.
- **Crossover**: **dos puntos** (two-point) y **reparación**. El intercambio de subcadenas puede producir duplicados; el operador de reparación reemplaza genes duplicados por los trabajos faltantes, en el orden en que aparecen.
- **Mutación**: **inversión** (inversion mutation): se eligen dos posiciones y se invierte el segmento.

### Parámetros sugeridos por el paper
- **Población**: 60
- **Probabilidad de cruce**: 0.80
- **Probabilidad de mutación**: 0.15
- **Criterio de parada**: $50(n+m)$ iteraciones

## Implementación
- `GA.h` / `GA.cpp`: implementación del algoritmo genético.
- `Instances.h`: cinco instancias benchmark de Taillard usadas también en el primer corte (`20x5`, `20x10`, `50x10`, `100x10`, `100x20`).
- `main_ga.cpp`: ejecuta el GA sobre las 5 instancias y reporta el mejor makespan y la secuencia.

## Nota sobre las instancias
Las matrices se mantienen idénticas a las usadas en el primer corte para permitir comparación directa.
