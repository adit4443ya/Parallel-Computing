import json
import time


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


def iterativeBacktrackMIS(graph):
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

    start = time.time()
    maximalIndependentSet = iterativeBacktrackMIS(graph)
    end = time.time()

    print("Maximal Independent Set (Iterative Backtracking):", maximalIndependentSet)
    print("Time taken:", round((end - start) * 1000, 2), "milliseconds")
