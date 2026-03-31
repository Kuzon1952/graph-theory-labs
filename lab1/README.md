# Graph Theory — Lab 1

**Variant:** 36  
**Topic:** Graph generation using the Normal distribution  
**Reference:** V. Vadzinsky  

---

## 📌 Description

This project implements generation and analysis of random graphs in C++.

The program can generate either:

- Undirected connected acyclic graph (tree)  
- Directed acyclic graph (DAG)  

using values derived from a normal-distribution-based random generator.

---

## ⚙️ Features

After generation, the program supports:

- Adjacency matrix / adjacency list / edge list output  
- Eccentricity computation  
- Radius and diameter computation  
- Center and diametral vertices detection  
- Shimbell’s method:
  - Minimum-path matrix  
  - Maximum-path matrix  
- Route search:
  - Existence of path  
  - Number of routes  
  - Printing all simple routes  

---

## 🗂️ Project Structure

lab1/

├── main.cpp  
├── graph.cpp / graph.h  
├── distribution.cpp / distribution.h  
├── tree_generator.cpp / tree_generator.h  
├── eccentricity.cpp / eccentricity.h  
├── shimbell.cpp / shimbell.h  
├── path_counter.cpp / path_counter.h  
├── CMakeLists.txt  

Executable: graph_lab

---

## 📐 Mathematical Background

### Normal Distribution Approximation

x = sqrt(12/n) * (sum(r_i) - n/2)

Where:

- r_i ~ U(0,1)  
- n = 12 (default)

---

## 🌳 Graph Generation

### Undirected Graph (Tree)

1. Generate degree sequence using Normal distribution  
2. Ensure:
   sum(d_i) = 2(n - 1)  
3. Build tree using degree-proportional attachment  

✔ Connected  
✔ Acyclic  

---

### Directed Graph (DAG)

1. Generate random topological order  
2. Add spanning path  
3. Add extra forward edges using Normal distribution  

✔ Acyclic  

---

## 🔁 Graph Types

### Undirected Tree
- Symmetric matrix  
- One path between any two vertices  

### Directed Graph
- Asymmetric matrix  
- No cycles  

---

## 📊 Algorithms

### Graph Output
- Adjacency list  
- Edge list  
- Adjacency matrix  
- Weight matrix  

---

### Graph Metrics

Distance matrix  

Eccentricity:
e(v) = max distance  

Radius:
r = min e(v)  

Diameter:
D = max e(v)  

---

### Shimbell Method

- Minimum path matrix  
- Maximum path matrix  

---

### Route Search

- Path existence  
- Number of routes  
- All paths printed (DFS)  

---

## 📋 Menu

1. Generate graph  
2. Show graph  
3. Eccentricity / radius / diameter  
4. Shimbell method  
5. Route search  
0. Exit  

---

## ▶️ Build and Run

### CMake

cmake -S . -B build  
cmake --build build  

Run:

.\build\graph_lab.exe  

---

### g++

g++ -std=c++17 main.cpp graph.cpp distribution.cpp tree_generator.cpp eccentricity.cpp shimbell.cpp path_counter.cpp -o graph_lab  

Run:

./graph_lab  

---

## 📝 Notes

- Vertices displayed from 1 (internally 0-based)  
- Supports positive / negative / mixed weights  
- Directed graphs are DAGs  

---

## 🚀 Status

✔ Modular structure  
✔ All algorithms implemented  
⚠️ TODO:
- Fix Shimbell zero handling  
- Improve graph density  
