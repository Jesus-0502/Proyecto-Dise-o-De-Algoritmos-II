# Cuarto Corte — PFSP

Este corte introduce una metaheurística propia inspirada en el comportamiento de una colonia de abejas y adaptada al **Permutation Flow Shop Scheduling Problem (PFSP)**.

## Implementación

La implementación principal está en:

- [bee-colony/ABC.h](bee-colony/ABC.h)
- [bee-colony/ABC.cpp](bee-colony/ABC.cpp)
- [bee-colony/main_abc.cpp](bee-colony/main_abc.cpp)

La idea central es combinar:

- **fuentes de alimento** = secuencias factibles del PFSP,
- **abejas empleadas** = exploración local guiada entre permutaciones,
- **abejas observadoras** = intensificación sobre las mejores soluciones,
- **abejas exploradoras** = reinicio de soluciones estancadas.

Además, el algoritmo usa:

- inicialización con **NEH** y variantes perturbadas,
- búsqueda local por **inserción muestreada**,
- **path relinking** periódico entre soluciones elite.

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
