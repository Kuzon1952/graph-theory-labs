\# Graph Theory — Lab 1  

\*\*Variant:\*\* 36  

\*\*Topic:\*\* Graph generation using Normal (Gaussian) distribution  

\*\*Reference:\*\* V. Vadzinsky (p.106 — Box-Muller method)



\---



\## 📌 Description



This project implements a graph theory laboratory assignment with the following features:



\- Graph generation using \*\*Normal distribution\*\*

\- Construction of a \*\*tree using vertex degrees and Prufer sequence\*\*

\- Conversion to \*\*directed graph\*\*

\- Analysis of graph properties:

&#x20; - Eccentricity

&#x20; - Radius

&#x20; - Diameter

&#x20; - Center vertices

\- \*\*Shimbel method\*\* for path analysis:

&#x20; - Minimum path matrix

&#x20; - Maximum path matrix

\- \*\*Route existence and counting\*\* between vertices



\---



\## 📐 Mathematical Background



\### Normal Distribution (Box-Muller Method)



\\\[

x = \\sqrt{-2 \\ln(r\_1)} \\cdot \\cos(2\\pi r\_2)

\\]

\\\[

y = \\mu + \\sigma x

\\]



Where:

\- \\( r\_1, r\_2 \\in (0,1] \\)

\- \\( \\mu \\) — mean

\- \\( \\sigma \\) — standard deviation



\---



\## 🌳 Graph Generation



\### Steps:

1\. Generate vertex degrees using Normal distribution

2\. Adjust degrees so that:

&#x20;  \\\[

&#x20;  \\sum deg(v) = 2(n - 1)

&#x20;  \\]

3\. Build a \*\*Prufer sequence\*\*

4\. Decode it into a \*\*tree\*\*

5\. Randomly assign directions → directed graph



\---



\## 🔁 Graph Modes



\- \*\*Undirected tree\*\*

&#x20; - Symmetric adjacency matrix

&#x20; - Exactly one path between any two vertices



\- \*\*Directed graph\*\*

&#x20; - Asymmetric adjacency matrix

&#x20; - Edges randomly oriented



\---



\## ⚙️ Features



\### 1. Graph Generation

\- Uses Gaussian distribution for vertex degrees

\- Ensures valid tree structure

\- Automatically builds adjacency list and matrix



\---



\### 2. Graph Visualization

\- Adjacency List

\- Adjacency Matrix

\- Edge List



\---



\### 3. Graph Properties



\- Distance matrix (BFS)

\- Eccentricity of each vertex:

&#x20; \\\[

&#x20; e(v) = \\max d(v, u)

&#x20; \\]



\- Radius:

&#x20; \\\[

&#x20; r = \\min e(v)

&#x20; \\]



\- Diameter:

&#x20; \\\[

&#x20; D = \\max e(v)

&#x20; \\]



\- Center vertices:

&#x20; \\\[

&#x20; e(v) = r

&#x20; \\]



\- Diametrical vertices:

&#x20; \\\[

&#x20; e(v) = D

&#x20; \\]



\---



\### 4. Shimbel Method



Used to compute:



\- \*\*Minimum path matrix\*\* (shortest paths)

\- \*\*Maximum path matrix\*\* (longest simple paths in tree)



Matrix definition:



\\\[

D^{(k)}\[i]\[j] = \\min\_m \\left( D^{(k-1)}\[i]\[m] + W\[m]\[j] \\right)

\\]



\---



\### 5. Route Search



\- Finds \*\*all simple paths\*\* between two vertices

\- Uses DFS with backtracking

\- Outputs:

&#x20; - Existence of path

&#x20; - Number of routes

&#x20; - All routes



\---



\## ▶️ How to Run



\### 1. Compile



```bash

g++ main.cpp -o graph

