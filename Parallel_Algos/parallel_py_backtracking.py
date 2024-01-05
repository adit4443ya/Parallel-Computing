import json
import time
from multiprocessing import Pool


class Graph:
    def __init__(self, numVertices):
        self.numVertices = numVertices
        self.adjacencyList = [set() for _ in range(numVertices)]

    def addEdge(self, u, v):
        self.adjacencyList[u].add(v)
        self.adjacencyList[v].add(u)

    def isAdjacentToSet(self, vertex, independentSet):
        for v in independentSet:
            if v in self.adjacencyList[vertex]:
                return True
        return False

    def getNumVertices(self):
        return self.numVertices

    def getAdjacencyList(self, vertex):
        return self.adjacencyList[vertex]


def findMISWorker(args):
    graph, comb_size = args  # Unpack the arguments
    numVertices = graph.getNumVertices()
    maximalIndependentSet = set()

    stack = [(set(), 0)]

    while stack:
        currentSet, currentVertex = stack.pop()

        if currentVertex == numVertices:
            if len(currentSet) > len(maximalIndependentSet):
                maximalIndependentSet = currentSet
            continue

        if not graph.isAdjacentToSet(currentVertex, currentSet):
            withCurrent = currentSet | {currentVertex}
            stack.append((withCurrent, currentVertex + 1))

        withoutCurrent = currentSet
        stack.append((withoutCurrent, currentVertex + 1))

    return maximalIndependentSet
if __name__ == "__main__":
    with open("random_graphS.json", "r") as input_file:
        jsonData = json.load(input_file)

    numVertices = jsonData["num_vertices"]
    graph = Graph(numVertices)

    for edge in jsonData["edges"]:
        graph.addEdge(edge[0], edge[1])

    # Number of threads (adjust as needed)
    num_threads = 4
    pool = Pool(num_threads)

    start = time.time()

    # Generate different combination sizes for parallel processing
    comb_sizes = range(1, numVertices + 1)
    
    # Use a list of tuples to pass arguments to worker processes
    args_list = [(graph, comb_size) for comb_size in comb_sizes]

    # Parallelize the computation and unpack the results
    maximalIndependentSets = pool.map(findMISWorker, args_list)

    # Find the largest MIS from the results
    maximalIndependentSet = max(maximalIndependentSets, key=lambda s: len(s))

    end = time.time()

    print("Maximal Independent Set (Iterative Backtracking):", maximalIndependentSet)
    print("Time taken:", round((end - start) * 1000, 2), "milliseconds")
