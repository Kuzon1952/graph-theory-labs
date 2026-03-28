# Graph Theory — Lab 1  

**Variant:** 36  
**Topic:** Graph generation using Normal (Gaussian) distribution  
**Reference:** V. Vadzinsky (p.106 — Box-Muller method)

---

## 📌 Description

This project implements:

- Graph generation using **Normal distribution**
- Construction of a **tree using vertex degrees and Prufer sequence**
- Conversion to **directed graph**
- Analysis of graph properties:
  - Eccentricity
  - Radius
  - Diameter
  - Center vertices
- **Shimbel method**:
  - Minimum path matrix
  - Maximum path matrix
- **Route search**:
  - Existence of path
  - Number of routes

---

## 📐 Mathematical Background

### Normal Distribution (Box-Muller)

x = √(-2 ln(r₁)) · cos(2π r₂)

y = μ + σx

Where:
- r₁, r₂ ∈ (0,1]
- μ — mean
- σ — standard deviation

---

## 🌳 Graph Generation

1. Generate vertex degrees using Normal distribution  
2. Ensure:
   sum of degrees = 2(n - 1)  
3. Build Prufer sequence  
4. Convert to tree  
5. Assign directions → directed graph  

---

## 🔁 Graph Types

### Undirected Tree
- Symmetric matrix  
- One path between any vertices  

### Directed Graph
- Asymmetric matrix  
- Random directions  

---

## ⚙️ Features

### Graph Generation
- Uses Gaussian distribution  
- Builds adjacency matrix and list  

### Graph Properties
- Distance matrix  
- Eccentricity:
  e(v) = max distance  

- Radius:
  r = min e(v)  

- Diameter:
  D = max e(v)  

---

## ▶️ How to Run

Compile:

g++ main.cpp -o graph

Run:

./graph
