import json
import networkx as nx
import numpy as np


def save_graph_to_json(graph, filename):
    num_nodes = graph.number_of_nodes()
    edges = list(graph.edges())
    num_edges = graph.number_of_edges() 
    adjacency_matrix = nx.to_numpy_array(graph, dtype=int)

    with open(filename, "w") as file:
        json.dump({
            "num_vertices": num_nodes,
            "num_edges": num_edges,
            "edges": edges,
            "adjacency_matrix": adjacency_matrix.tolist()
        }, file, indent=4)


num_nodes = 500 # Change to the desired number of nodes
edge_probability = 0.7  # Adjust the edge creation probability as needed

random_graph = nx.gnp_random_graph(num_nodes, edge_probability)
json_filename = "random_graphP.json"
save_graph_to_json(random_graph, json_filename)
print(f"Random graph with {num_nodes} nodes saved to {json_filename}")
