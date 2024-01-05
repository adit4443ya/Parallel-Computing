#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
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

set<int> greedyMIS(const Graph& graph) {
    set<int> maximalIndependentSet;

    vector<bool> selected(graph.getNumVertices(), false);

    for (int v = 0; v < graph.getNumVertices(); ++v) {
        if (!selected[v]) {
            maximalIndependentSet.insert(v);
            selected[v] = true;

            for (int neighbor : graph.getAdjacencyList(v)) {
                selected[neighbor] = true;
            }
        }
    }

    return maximalIndependentSet;
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
 auto start = std::chrono::high_resolution_clock::now();
    set<int> maximalIndependentSet = greedyMIS(graph);
 auto end = std::chrono::high_resolution_clock::now();
    cout<<endl;

    cout << "Maximal Independent Set: (Greedy)";
    for (int v : maximalIndependentSet) {
        cout << v << " ";
    }
    cout << endl;
 chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;
    cout<<endl;

    return 0;
}
