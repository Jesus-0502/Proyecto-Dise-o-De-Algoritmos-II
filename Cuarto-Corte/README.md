# Cuarto Corte — PFSP

Este corte introduce una metaheuristica propia llamada **Propagacion de Rumores con Relinking Guiado (PR-RG)** y adaptada al **Permutation Flow Shop Scheduling Problem (PFSP)**.

## Implementación

La implementación principal está en:

- [bee-colony/ABC.h](bee-colony/ABC.h)
- [bee-colony/ABC.cpp](bee-colony/ABC.cpp)
- [bee-colony/main_abc.cpp](bee-colony/main_abc.cpp)

La idea central es combinar:

- **rumores** = secuencias factibles del PFSP,
- **informantes activos** = exploracion local guiada entre permutaciones,
- **oyentes** = intensificacion sobre los rumores mas prometedores,
- **informantes exploradores** = reinicio de rumores estancados.

Además, el algoritmo usa:

- inicializacion hibrida con **NEH perturbado (75%)** y aleatorio (**25%**),
- busqueda local por **insercion muestreada**,
- **path relinking** periodico entre rumores elite,
- seleccion por ruleta sesgada por calidad (makespan).

## Compilación

```bash
cd Cuarto-Corte/bee-colony
make
```

## Ejecución

```bash
./abc_pfsp
```

El programa ejecuta la metaheurística sobre las cinco instancias Taillard reutilizadas desde el segundo corte y reporta el makespan, el gap y el tiempo de ejecución para varias configuraciones.

Nota: por compatibilidad historica, los nombres de archivos y ejecutable conservan la nomenclatura `ABC` (`ABC.h`, `ABC.cpp`, `main_abc.cpp`, `abc_pfsp`), pero la interpretacion metodologica del corte corresponde a **PR-RG**.
