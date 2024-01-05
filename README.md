# High Performance Computing using CPU Parallelisation 

## Introduction
This repository documents the exploration of graph-based algorithms and their application to solving complex problems, focusing specifically on the Maximal Independent Sets (MIS) problem and the implementation of parallel computing techniques.

## Main Work

This repo includes some of the amazing algorithms for solving the NP-hard problem which is finding the Maximal Independent Sets (MIS) for Graphs.
- It includes serial implementations of algorithms on undirected graphs
- It also include thier parallel implementation on multiple cores which can be set by doing small modification as per your system
- It also includes some of the most enlightening research papers for the same which can give references

All the Algorithms are designed manually.(except ECL)

## Running the code
#### Steps
- Extract this files
- Download MINGW and OpenMP to get started
- First run Random_graph.py file by manually changing the two  parameters :- num_nodes and edge_probability
- then compile any desired algorithm via this command:- `> g++ -fopenmp filename.cpp -o l`
- then run via this command:-`> ./l`
- you will get output after required time containing the time taken and the set
