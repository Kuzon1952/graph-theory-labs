# Graph Theory — Lab 1  

**Variant:** 36  

**Topic:** Graph generation using Normal (Gaussian) distribution  

**Reference:** V. Vadzinsky (p.106 — Box-Muller method)

---

## 📌 Description

This project implements a graph theory laboratory assignment with the following features:

- Graph generation using **Normal distribution**
- Construction of a **tree using vertex degrees and Prufer sequence**
- Conversion to **directed graph**
- Analysis of graph properties:
  - Eccentricity
  - Radius
  - Diameter
  - Center vertices

- **Shimbel method** for path analysis:
  - Minimum path matrix
  - Maximum path matrix

- **Route existence and counting** between vertices

---

## 📐 Mathematical Background

### Normal Distribution (Box-Muller Method)

\[
x = \sqrt{-2 \ln(r_1)} \cdot \cos(2\pi r_2)
\]

\[
y = \mu + \sigma x
\]

Where:
- \( r_1, r_2 \in (0,1] \)
- \( \mu \) — mean
- \( \sigma \) — standard deviation

---

## 🌳 Graph Generation

### Steps:
1. Generate vertex degrees using Normal distribution  
2. Adjust degrees so that:
\[
\sum deg(v) = 2(n - 1)
\]
3. Build a **Prufer sequence**  
4. Decode it into a **tree**  
5. Randomly assign directions → directed graph  

---

## 🔁 Graph Modes

- **Undirected tree**
  - Symmetric adjacency matrix
  - Exactly one path between any two vertices

- **Directed graph**
  - Asymmetric adjacency matrix
  - Edges randomly oriented

---

## ⚙️ Features

### 1. Graph Generation
- Uses Gaussian distribution for vertex degrees  
- Ensures valid tree structure  
- Builds adjacency list and matrix  

---

### 2. Graph Visualization
- Adjacency List  
- Adjacency Matrix  
- Edge List  

---

### 3. Graph Properties

- Distance matrix (BFS)

- Eccentricity:
\[
e(v) = \max d(v, u)
\]

- Radius:
\[
r = \min e(v)
\]

- Diameter:
\[
D = \max e(v)
\]

- Center:
\[
e(v) = r
\]

- Diameter vertices:
\[
e(v) = D
\]

---

### 4. Shimbel Method

\[
D^{(k)}[i][j] = \min_m \left( D^{(k-1)}[i][m] + W[m][j] \right)
\]

Used for:
- Shortest paths  
- Longest paths  

---

### 5. Route Search

- DFS with backtracking  
- Finds:
  - All paths  
  - Path existence  
  - Number of routes  

---

## ▶️ How to Run

### Compile
```bash
g++ main.cpp -o graph
