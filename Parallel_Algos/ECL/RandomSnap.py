import json
import networkx as nx


def save_snap_graph_to_json(graph, filename):
    edges = list(graph.edges())
    nodes=graph.number_of_nodes()
    edg=graph.number_of_edges()
    with open(filename, "w") as file:
        file.write(f"# Nodes: {nodes} Edges: {edg}\n")
        for edge in edges:
            file.write(f"{edge[0]} {edge[1]}\n")

    print(
        f"Snap graph with {graph.number_of_nodes()} nodes saved to {filename}")


# Creating a graph
num_nodes = 350  # Change to the desired number of nodes
edge_probability = 0.3  # Adjust the edge creation probability as needed
random_graph = nx.gnp_random_graph(num_nodes, edge_probability)

# Storing the graph in snap format in a JSON file
# Update the file path and name as needed
json_filename = "random_graph_snap.txt"
save_snap_graph_to_json(random_graph, json_filename)
