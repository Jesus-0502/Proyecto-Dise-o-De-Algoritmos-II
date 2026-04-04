# Informe del Cuarto Corte

## Optimización del PFSP con una Colonia de Abejas con Relinking Guiado

### Introducción

En este cuarto corte se propuso una metaheurística propia inspirada en el comportamiento de una **colonia de abejas** y adaptada al **Permutation Flow Shop Scheduling Problem (PFSP)**. La idea fue tomar un proceso natural sencillo de explicar —búsqueda de alimento, comunicación de calidad mediante danza y abandono de fuentes pobres— y convertirlo en una estrategia de exploración e intensificación para permutaciones.

El PFSP continúa siendo el mismo problema trabajado en los cortes anteriores: encontrar una secuencia de trabajos que minimice el *makespan*. La ventaja de trabajar sobre el mismo benchmark es que la comparación es directa con los algoritmos anteriores: **ACO**, **algoritmo memético** y **Scatter Search**.

### Inspiración y formulación de la metaheurística

La metaheurística desarrollada se puede resumir como una **Colonia de Abejas con Relinking Guiado (ABC-RG)**. Cada solución del problema corresponde a una fuente de alimento. Su calidad se mide por el valor de $C_{\max}$, es decir, el *makespan*.

La dinámica general es la siguiente:

1. **Inicialización**: se generan soluciones mediante NEH perturbado y, en menor proporción, permutaciones aleatorias.
2. **Abejas empleadas**: cada solución intenta mejorar guiándose por otra solución de referencia más prometedora.
3. **Abejas observadoras**: se asignan más intentos a las mejores fuentes usando selección por ruleta.
4. **Abejas exploradoras**: si una solución no mejora durante demasiados intentos, se reemplaza por una nueva fuente.
5. **Relinking periódico**: de forma ocasional se conecta la mejor solución con otra elite para explorar trayectorias intermedias.

La función objetivo para el PFSP es:

$$
\min_{\pi} C_{\max}(\pi)
$$

donde $\pi$ es una permutación de trabajos y $C_{\max}$ es el tiempo de finalización del último trabajo en la última máquina.

### Adaptación al PFSP

La adaptación al PFSP fue natural porque el espacio de búsqueda está formado por permutaciones. En vez de mover variables continuas, cada abeja manipula una secuencia de trabajos. Las operaciones fundamentales fueron:

- **inserción** de un trabajo en otra posición,
- **inversión** de un segmento,
- **búsqueda local por inserción muestreada**,
- **NEH** como constructor base,
- **path relinking** entre permutaciones.

El código principal está en [bee-colony/ABC.cpp](bee-colony/ABC.cpp), mientras que la interfaz de parámetros y resultados está en [bee-colony/ABC.h](bee-colony/ABC.h). El ejecutable se controla desde [bee-colony/main_abc.cpp](bee-colony/main_abc.cpp).

### Adaptación conceptual a otros problemas conocidos

Aunque la implementación concreta se ejecutó sobre PFSP, la idea también se adapta de forma directa a otros problemas de clase:

- **TSP**: cada fuente de alimento sería una ruta hamiltoniana; la calidad se mediría por la distancia total del tour.
- **QAP**: cada solución sería una asignación de instalaciones a ubicaciones; la calidad se mediría por costo total.
- **Knapsack**: el esquema de exploración y abandono puede reutilizarse, aunque la representación ya no sería una permutación sino un subconjunto de objetos.

En este trabajo, el caso principal y evaluado experimentalmente fue PFSP.

### Parámetros ajustables

Los parámetros más importantes de la implementación son:

- tamaño de la colonia,
- número de iteraciones,
- número de élites preservadas,
- límite de abandono de una fuente,
- cantidad de pasos de búsqueda local,
- cantidad de observadoras,
- frecuencia de relinking,
- probabilidad de reinicio aleatorio.

Estos parámetros controlan el equilibrio entre exploración e intensificación.

### Oportunidades de paralelización

El algoritmo tiene varias oportunidades claras de paralelización:

- **Tipo 1**: evaluación independiente de las soluciones de la colonia.
- **Tipo 2**: generación de vecinos para abejas empleadas y observadoras en paralelo.
- **Tipo 3**: ejecución simultánea de diferentes configuraciones, semillas o instancias del benchmark.

### Benchmark y configuración experimental

Se reutilizaron exactamente las cinco instancias Taillard del segundo corte:

- `pfsp_20x5`
- `pfsp_20x10`
- `pfsp_50x10`
- `pfsp_100x10`
- `pfsp_100x20`

Se mantuvieron los mismos valores de referencia usados en el proyecto:

- 1278
- 1582
- 3037
- 5776
- 6330

La ejecución se realizó con tres configuraciones del algoritmo ABC-RG. La configuración más intensa fue la que obtuvo los mejores resultados globales.

### Resultados obtenidos

#### Mejor resultado por instancia

| Instancia | Referencia | Mejor ABC-RG | Gap % | Tiempo (s) |
|---|---:|---:|---:|---:|
| pfsp_20x5 | 1278 | 1278 | 0.00 | 0.275652 |
| pfsp_20x10 | 1582 | 1582 | 0.00 | 0.865191 |
| pfsp_50x10 | 3037 | 3067 | 0.99 | 3.409660 |
| pfsp_100x10 | 5776 | 5716 | -1.04 | 12.516500 |
| pfsp_100x20 | 6330 | 6548 | 3.44 | 15.628500 |

Con estos valores, el promedio aproximado del mejor ABC-RG fue:

- **Gap promedio**: $0.68\%$
- **Tiempo promedio**: $6.54$ s

#### Comparación global con los cortes anteriores

| Algoritmo | Gap promedio | Tiempo promedio |
|---|---:|---:|
| ACO | 3.19% | 0.31 s |
| Memético | -0.31% | 25.59 s |
| Scatter Search | -0.17% | 29.33 s |
| ABC-RG | 0.68% | 6.54 s |

### Discusión

El ABC-RG queda claramente por encima del ACO en calidad y es mucho más rápido que el memético y Scatter Search. En instancias pequeñas incluso alcanza el óptimo y, en la instancia `pfsp_50x10`, se mantiene cerca del valor de referencia. En `pfsp_100x10` queda por debajo del valor de referencia, aunque todavía por encima de los mejores métodos híbridos del tercer corte, por lo que sigue siendo una solución intermedia muy competitiva.

La lectura general es que la estrategia de abejas funciona bien como compromiso entre rapidez y calidad, pero el refinamiento intensivo del memético sigue siendo superior cuando el objetivo principal es empujar el makespan lo más cerca posible del óptimo.

### Conclusiones

La metaheurística propuesta cumple con el objetivo del cuarto corte: es una heurística propia, inspirada en un proceso natural, implementada sobre el PFSP y comparada contra los resultados obtenidos en entregas anteriores.

Las principales lecciones fueron:

- NEH sigue siendo una base muy fuerte para PFSP.
- La exploración tipo colonia ayuda a mantener diversidad.
- El relinking guiado mejora la búsqueda sin perder factibilidad.
- El balance entre calidad y tiempo es mejor que en ACO, aunque no alcanza la precisión del memético.

En síntesis, ABC-RG ofrece una solución intermedia muy útil: más rápida que los híbridos pesados y más competitiva que la colonia de hormigas base.
