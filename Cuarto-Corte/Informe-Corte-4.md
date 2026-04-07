# Informe del Cuarto Corte

## Optimizacion del PFSP con Propagacion de Rumores con Relinking Guiado

### Introducción

En este cuarto corte se propuso una metaheuristica propia inspirada en una **red de informantes** y adaptada al **Permutation Flow Shop Scheduling Problem (PFSP)**. La idea fue tomar un proceso social sencillo de explicar - difusion de rumores, refinamiento de chismes prometedores y abandono de historias estancadas - y convertirlo en una estrategia de exploracion e intensificacion para permutaciones.

El PFSP continúa siendo el mismo problema trabajado en los cortes anteriores: encontrar una secuencia de trabajos que minimice el *makespan*. La ventaja de trabajar sobre el mismo benchmark es que la comparación es directa con los algoritmos anteriores: **ACO**, **algoritmo memético** y **Scatter Search**.

### Inspiración y formulación de la metaheurística

La metaheuristica desarrollada se puede resumir como una **Propagacion de Rumores con Relinking Guiado (PR-RG)**. Cada solucion del problema corresponde a un rumor. Su calidad se mide por el valor de $C_{\max}$, es decir, el *makespan*.

La dinámica general es la siguiente:

1. **Inicializacion hibrida**: se generan rumores con 75% de soluciones NEH perturbadas y 25% de permutaciones aleatorias.
2. **Informantes activos**: cada rumor intenta mejorar guiandose por un rumor elite de referencia.
3. **Seleccion de oyentes**: se asignan mas intentos a los mejores rumores usando seleccion por ruleta sesgada por calidad.
4. **Informantes exploradores**: cuando un rumor no mejora durante demasiados intentos (contador de aburrimiento), se reemplaza por informacion fresca.
5. **Relinking periodico**: de forma ocasional se conecta el mejor rumor con otro elite para explorar trayectorias intermedias.

La función objetivo para el PFSP es:

$$
\min_{\pi} C_{\max}(\pi)
$$

donde $\pi$ es una permutación de trabajos y $C_{\max}$ es el tiempo de finalización del último trabajo en la última máquina.

### Adaptación al PFSP

La adaptacion al PFSP fue natural porque el espacio de busqueda esta formado por permutaciones. En vez de mover variables continuas, cada informante manipula una secuencia de trabajos. Las operaciones fundamentales fueron:

- **inserción** de un trabajo en otra posición,
- **inversión** de un segmento,
- **búsqueda local por inserción muestreada**,
- **NEH** como constructor base de rumores,
- **path relinking** entre permutaciones.

El codigo principal esta en [bee-colony/ABC.cpp](bee-colony/ABC.cpp), mientras que la interfaz de parametros y resultados esta en [bee-colony/ABC.h](bee-colony/ABC.h). El ejecutable se controla desde [bee-colony/main_abc.cpp](bee-colony/main_abc.cpp). Aunque los nombres de archivos conservan la nomenclatura historica de ABC, la logica metodologica del informe corresponde a PR-RG.

### Adaptación conceptual a otros problemas conocidos

Aunque la implementación concreta se ejecutó sobre PFSP, la idea también se adapta de forma directa a otros problemas de clase:

- **TSP**: cada rumor seria una ruta hamiltoniana; la calidad se mediria por la distancia total del tour.
- **QAP**: cada solución sería una asignación de instalaciones a ubicaciones; la calidad se mediría por costo total.
- **Knapsack**: el esquema de exploración y abandono puede reutilizarse, aunque la representación ya no sería una permutación sino un subconjunto de objetos.

En este trabajo, el caso principal y evaluado experimentalmente fue PFSP.

### Parámetros ajustables

Los parámetros más importantes de la implementación son:

- tamano de la red de informantes,
- número de iteraciones,
- numero de elites preservadas,
- limite de aburrimiento para abandonar un rumor estancado,
- cantidad de pasos de busqueda local,
- cantidad de oyentes,
- frecuencia de relinking,
- probabilidad de reinicio aleatorio.

Estos parámetros controlan el equilibrio entre exploración e intensificación.

### Oportunidades de paralelización

El algoritmo tiene varias oportunidades claras de paralelización:

- **Tipo 1**: evaluacion independiente de los rumores de la red.
- **Tipo 2**: generacion de vecinos para informantes activos y oyentes en paralelo.
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

La ejecucion se realizo con tres configuraciones del algoritmo PR-RG. La configuracion mas robusta (Configuracion #3) fue la que obtuvo los mejores resultados globales y el mejor equilibrio entre calidad y tiempo.

### Resultados obtenidos

#### Mejor resultado por instancia

| Instancia | Referencia | Mejor PR-RG | Gap % | Tiempo (s) |
|---|---:|---:|---:|---:|
| pfsp_20x5 | 1278 | 1278 | 0.00 | 0.275652 |
| pfsp_20x10 | 1582 | 1582 | 0.00 | 0.865191 |
| pfsp_50x10 | 3037 | 3067 | 0.99 | 3.409660 |
| pfsp_100x10 | 5776 | 5716 | -1.04 | 12.516500 |
| pfsp_100x20 | 6330 | 6548 | 3.44 | 15.628500 |

Con estos valores, el promedio aproximado del mejor PR-RG fue:

- **Gap promedio**: $0.68\%$
- **Tiempo promedio**: $6.54$ s

#### Comparación global con los cortes anteriores

| Algoritmo | Gap promedio | Tiempo promedio |
|---|---:|---:|
| ACO | 3.19% | 0.31 s |
| Memético | -0.31% | 25.59 s |
| Scatter Search | -0.17% | 29.33 s |
| PR-RG | 0.68% | 6.54 s |

### Discusión

El PR-RG queda claramente por encima del ACO en calidad y es mucho mas rapido que el memetico y Scatter Search. En instancias pequenas incluso alcanza el optimo y, en la instancia `pfsp_50x10`, se mantiene cerca del valor de referencia. En `pfsp_100x10` queda por debajo del valor de referencia, aunque todavia por encima de los mejores metodos hibridos del tercer corte, por lo que sigue siendo una solucion intermedia muy competitiva.

La lectura general es que la estrategia de propagacion de rumores funciona bien como compromiso entre rapidez y calidad, pero el refinamiento intensivo del memetico sigue siendo superior cuando el objetivo principal es empujar el makespan lo mas cerca posible del optimo.

### Conclusiones

La metaheurística propuesta cumple con el objetivo del cuarto corte: es una heurística propia, inspirada en un proceso natural, implementada sobre el PFSP y comparada contra los resultados obtenidos en entregas anteriores.

Las principales lecciones fueron:

- NEH sigue siendo una base muy fuerte para PFSP.
- La difusion de rumores guiada por elites ayuda a mantener diversidad con direccion.
- El relinking guiado mejora la búsqueda sin perder factibilidad.
- El balance entre calidad y tiempo es mejor que en ACO, aunque no alcanza la precisión del memético.

En sintesis, PR-RG ofrece una solucion intermedia muy util: significativamente mas rapida que los hibridos pesados y mucho mas precisa que la colonia de hormigas base.
