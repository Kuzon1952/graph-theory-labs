# Graph Theory Labs (SPbSTU, 2nd year 2nd sem., Spring 2025/2026)

This repository contains laboratory works for the **Graph Theory** course at Peter the Great St. Petersburg Polytechnic University.

**Course portal:** https://tema.spbstu.ru/tgraph/

## Variants

| Lab | Variant |
|-----|---------|
| Lab 1 | Normal distribution |
| Lab 2 | c (DFS), j (Floyd-Warshall) |
| Lab 3 | -- |
| Lab 4 | b (Boruvka), g (minimum edge cover) |
| Lab 5 | 36 (even) |
| Lab 6 | -- |

## Contents

---

### ЛАБОРАТОРНАЯ РАБОТА № 1

> 1. Сформировать случайным образом связный ациклический ориентированный граф в соответствии с заданным распределением (параметры распределения задаются как константы и подбираются экспериментально) с введенным пользователем количеством вершин. Распределение брать из справочника Вадзинского: либо реализовать блок-схему, либо генерирующую формулу. Генерация происходит по степеням вершин. В зависимости от дальнейших заданий будет использовать ориентированный или неориентированный граф, полученный из ориентированного. Можно реализовывать в консоли.
> 2. Посчитать эксцентриситеты вершин, выделить центр графа и диаметральные вершины.
> 3. Реализовать метод Шимбелла для сгенерированной с помощью заданного распределения весовой матрицы (пользователь вводит количество ребер), в ответе представить минимальный и/или максимальный путь (в виде матрицы). Весовая матрица генерируется также в соответствии с заданным распределением. При генерации необходимо учесть генерацию только положительных значений, только отрицательных значений и смешанную (на выбор пользователя).
> 4. Определить возможность построения маршрута от одной заданной точки до другой (вершины вводит пользователь) и подсчитать количество маршрутов.

**My variant:** Normal distribution

### Lab 1 -- Graph Generation & Properties
1. Random connected acyclic directed graph generation using **normal distribution** (Vadzinsky reference)
2. Vertex **eccentricities**, graph **center**, **diameter**, and diametral vertices
3. **Shimbell's method** -- min/max path matrices over *k* steps using a weight matrix
4. **Route existence and counting** between two user-specified vertices

---

### ЛАБОРАТОРНАЯ РАБОТА № 2

> 1. Для заданных графов (случайно сгенерированных в предыдущей работе) реализовать один из алгоритмов (см. варианты):
>    - a) упорядочить граф по алгоритму Фалкерсона,
>    - b) выполнить обход вершин графа поиском в ширину,
>    - c) выполнить обход вершин графа поиском в глубину,
>    - d) найти точки сочленения в графе,
>    - e) выполнить обход ребер графа поиском в ширину,
>    - f) выполнить обход ребер графа поиском в глубину.
>
> 2. Сгенерировать весовую матрицу (положительную или с отрицательными весами -- выбирает пользователь) и найти кратчайший путь для двух выбранных пользователем вершин. В результате выводится не только расстояние, но и сам путь в виде последовательности вершин. Для алгоритмов Дейкстры и Беллмана-Форда необходимо выводить вектор расстояний, для Флойда-Уоршалла -- матрицу расстояний.
>    - g) классический алгоритм Дейкстры,
>    - h) алгоритм Дейкстры для отрицательных весов,
>    - i) алгоритм Беллмана-Форда,
>    - j) алгоритм Флойда-Уоршалла.
>
> 3. Сравнить скорости работы реализованных алгоритмов (по количеству итераций).

**My variants:** c, j

### Lab 2 -- Traversal & Shortest Paths
1. Graph traversal algorithms (variants c, j):
   - **(c)** Vertex traversal via **DFS** (depth-first search)
   - **(j)** **Floyd-Warshall** algorithm -- shortest path distance matrix and path reconstruction
2. **Performance comparison** of implemented algorithms by iteration count

---

### ЛАБОРАТОРНАЯ РАБОТА № 3

> 1. На основе сгенерированного ориентированного графа получить случайным образом матрицы пропускных способностей и стоимости.
> 2. Для полученного графа найти максимальный поток по алгоритму Форда-Фалкерсона (или любого алгоритма из перечисленных в лекции).
> 3. Вычислить заданный поток минимальной стоимости (в качестве величины потока брать значение, равное [2/3*max], где max -- максимальный поток). Для этого использовать ранее реализованные алгоритмы Дейкстры, Беллмана-Форда или Флойда-Уоршалла.

**My variant:** --

### Lab 3 -- Network Flows
1. Generate **capacity** and **cost matrices** from the directed graph
2. **Ford-Fulkerson** algorithm for maximum flow
3. **Min-cost flow** with flow value = floor(2/3 * max_flow), using shortest-path augmentation (Dijkstra / Bellman-Ford / Floyd-Warshall)

---

### ЛАБОРАТОРНАЯ РАБОТА № 4

> 1. Подсчитать количество остовных деревьев, используя матричную теорему Кирхгофа.
> 2. Построить минимальный по весу остов для сгенерированного (неориентированного) взвешенного графа, используя алгоритм:
>    - a) Краскала;
>    - b) Борувки;
>    - c) Прима.
>
> Полученный остов закодировать с помощью кода Прюфера и декодировать его. Сохранять веса при кодировании обязательно.
>
> 3. Реализовать на исходном графе и полученном остове (по выбору пользователя) алгоритм:
>    - d) нахождения максимального независимого множества вершин;
>    - e) нахождения максимального независимого множества ребер;
>    - f) нахождения минимального вершинного покрытия;
>    - g) нахождения минимального реберного покрытия;
>    - h) нахождения минимальной раскраски графа.

**My variants:** b, g

### Lab 4 -- Trees & Coverings
1. **Kirchhoff's theorem** -- count the number of spanning trees (matrix-tree theorem)
2. **Minimum spanning tree** using **(b) Boruvka's algorithm**
3. **Prufer code** -- encode the MST into a Prufer sequence and decode it back, preserving edge weights
4. **(g) Minimum edge cover** on the original graph or on the MST

---

### ЛАБОРАТОРНАЯ РАБОТА № 5

> 1. Для заданных неориентированных графов (случайно сгенерированных в первой работе) проверить, является ли граф эйлеровым. Если нет, то модифицировать граф (логировать, что изменено). Построить эйлеров цикл.
>
> 2. Для заданных неориентированных графов (случайно сгенерированных в первой работе) построить кратчайший остов (алгоритмом из четвертой работы). На основе данного остова получить:
>    - *для нечетных вариантов:* фундаментальную систему циклов (вывести ее на экран), получение всех остальных циклов на основе фундаментальных с помощью операции симметрической разности (выбранные циклы вводит пользователь).
>    - *для четных вариантов:* фундаментальную систему разрезов (вывести ее на экран), получение всех остальных разрезов на основе фундаментальных с помощью операции симметрической разности (выбранные разрезы вводит пользователь).

**My variant:** 36 (even)

### Lab 5 -- Euler Cycles & Cutsets
1. Check if the undirected graph is **Eulerian**; if not, modify it (log changes); build an **Euler cycle**
2. Build the **shortest spanning tree** (MST) using Boruvka (Lab 4)
3. **(even variant)** Construct the **fundamental system of cutsets** from the MST; derive all other cutsets via **symmetric difference** (user selects cutsets)

---

### ЛАБОРАТОРНАЯ РАБОТА № 6 (отдельный отчет, на оценку 4 или 5)

> 1. Для произвольного текста на русском языке построить словарь на основе красно-черных деревьев. Реализовать функции добавления, удаления и поиска слова. (НЕЛЬЗЯ использовать готовые структуры данных!!!). Добавить функцию полной очистки словаря и загрузки/дополнения словаря из текстового файла.
>
> 2. Хеш-функция выбирается произвольно (в отчете описать ее качество и устойчивость к коллизиям), кроме тривиальных (например, первой буквы слова). Добавить функцию полной очистки словаря и загрузки/дополнения словаря из текстового файла.

**My variant:** --

### Lab 6 -- Dictionary (separate report, graded 4 or 5)
1. **Red-Black tree** based dictionary for Russian text -- insert, delete, search
2. **Hash table** based dictionary with a custom hash function (collision-resistant, non-trivial)
3. Load/clear dictionary from a text file; full dictionary reset

---

## Project Structure

```
graph-theory-labs/
|-- CMakeLists.txt              # Root CMake (C++17)
|-- LICENSE
|-- README.md
|
|-- shared/                     # Common utilities (all labs 1-5)
|   |-- constants.h
|   |-- graph.cpp / graph.h
|   |-- distribution.cpp / distribution.h
|   |-- dag_generator.cpp / dag_generator.h
|   +-- weight_matrix.cpp / weight_matrix.h
|
|-- lab1/
|   |-- eccentricity.cpp / eccentricity.h
|   |-- shimbell.cpp / shimbell.h
|   +-- path_counter.cpp / path_counter.h
|
|-- lab2/
|   |-- dfs.cpp / dfs.h
|   +-- floyd_warshall.cpp / floyd_warshall.h
|
|-- lab3/
|   |-- ford_fulkerson.cpp / ford_fulkerson.h
|   +-- min_cost_flow.cpp / min_cost_flow.h
|
|-- lab4/
|   |-- boruvka.cpp / boruvka.h
|   |-- kirchhoff.cpp / kirchhoff.h
|   |-- prufer.cpp / prufer.h
|   +-- edge_cover.cpp / edge_cover.h
|
|-- lab5/
|   |-- eulerian.cpp / eulerian.h
|   +-- fundamental_cutsets.cpp / fundamental_cutsets.h
|
|-- lab_combined/               # Single interactive binary for Labs 1-5
|   |-- CMakeLists.txt
|   +-- main.cpp
|
|-- lab6/                       # Separate project (own CMake)
|   |-- code/
|   |   |-- CMakeLists.txt
|   |   |-- main.cpp
|   |   |-- rbtree.cpp / rbtree.h
|   |   |-- hashtable.cpp / hashtable.h
|   |   +-- textutils.cpp / textutils.h
|   +-- report/
|       +-- lab6_report.pdf
|
+-- Report_lab1_5/              # Combined report for Labs 1-5
    |-- english_report.pdf
    +-- russian_report.pdf
```

## Build & Run

### Labs 1--5 (combined interactive menu)

```bash
cmake -B build -S .
cmake --build build
./build/lab_combined/lab_combined
```

### Lab 6 (dictionary)

```bash
cd lab6/code
cmake -B build -S .
cmake --build build
./build/Lab6Dictionary_check
```

## Interactive Menu (Labs 1--5)

```
 1. Generate graph
 2. Show graph (adjacency list / matrix)
 3. Generate weight matrix
--- Lab 1 ---
 4. Eccentricities, center, diametral vertices
 5. Shimbell's method (min/max paths, k steps)
 6. Find all routes between two vertices
--- Lab 2 ---
 7. DFS traversal
 8. Shortest path: Floyd-Warshall
--- Lab 3 ---
 9. Generate capacity and cost matrices
10. Max flow: Ford-Fulkerson
11. Min-cost flow
--- Lab 4 ---
12. Kirchhoff: count spanning trees
13. Boruvka: minimum spanning tree
14. Minimum edge cover
15. Prufer encode / decode
--- Lab 5 ---
16. Eulerian check / modify / Euler cycle
17. Fundamental cutsets from MST
 0. Exit
```

## License

MIT
