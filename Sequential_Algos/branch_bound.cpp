#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <iomanip>

using namespace std;
using json = nlohmann::json;

class Graph {
public:
    Graph(int numVertices);
    void addEdge(int u, int v);
    bool isAdjacentToSet(int vertex, const set<int>& independentSet) const;
    int getNumVertices() const;
    const set<int>& getAdjacencyList(int vertex) const;

private:
    int numVertices;
    vector<set<int>> adjacencyList;
};

Graph::Graph(int numVertices) : numVertices(numVertices) {
    adjacencyList.resize(numVertices);
}

void Graph::addEdge(int u, int v) {
    adjacencyList[u].insert(v);
    adjacencyList[v].insert(u);
}

bool Graph::isAdjacentToSet(int vertex, const set<int>& independentSet) const {
    for (int v : independentSet) {
        if (adjacencyList[vertex].count(v)) {
            return true;
        }
    }
    return false;
}

int Graph::getNumVertices() const {
    return numVertices;
}

const set<int>& Graph::getAdjacencyList(int vertex) const {
    return adjacencyList[vertex];
}

set<int> branchAndBoundMIS(const Graph &graph, set<int> currentSet, int currentVertex) {
    if (currentVertex == graph.getNumVertices()) {
        return currentSet;
    }

    int nextVertex = currentVertex + 1;

    if (!graph.isAdjacentToSet(currentVertex, currentSet)) {
        set<int> withCurrent = currentSet;
        withCurrent.insert(currentVertex);
        set<int> withCurrentResult = branchAndBoundMIS(graph, withCurrent, nextVertex);

        set<int> withoutCurrentResult = branchAndBoundMIS(graph, currentSet, nextVertex);

        if (withCurrentResult.size() > withoutCurrentResult.size()) {
            return withCurrentResult;
        }
        return withoutCurrentResult;
    }

    return branchAndBoundMIS(graph, currentSet, nextVertex);
}

bool isIndependentSet(const Graph& graph, const set<int>& independentSet) {
    for (int v : independentSet) {
        for (int u : independentSet) {
            if (v != u && graph.isAdjacentToSet(v, {u})) {
                return false;  // Not an independent set
            }
        }
    }
    return true;
}

int main() {
    ifstream inputFile("random_graphS.json");
    if (!inputFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    json jsonData;
    inputFile >> jsonData;

    int numVertices = jsonData["num_vertices"];
    Graph graph(numVertices);

    for (const auto& edge : jsonData["edges"]) {
        graph.addEdge(edge[0], edge[1]);
    }

    inputFile.close();
    auto start = chrono::high_resolution_clock::now();
    set<int> maximalIndependentSet = branchAndBoundMIS(graph, set<int>(), 0);
    auto end = chrono::high_resolution_clock::now();
    
  if (isIndependentSet(graph, maximalIndependentSet)) {
        cout << "The set is an independent set." << endl;
    } else {
        cout << "The set is NOT an independent set." << endl;
    }

    cout << "Maximal Independent Set (Branch and Bound): ";
    for (int v : maximalIndependentSet) {
        cout << v << " ";
    }
    cout << endl;

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;

    return 0;
}
