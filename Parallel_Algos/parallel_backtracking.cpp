#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <bits\json.hpp>
#include <chrono>
#include <omp.h>

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
set<int> parallelMIS(const Graph& graph, const set<int>& independentSet, int currentVertex) {
    set<int> newIndependentSet = independentSet;

    if (currentVertex == graph.getNumVertices()) {
        return newIndependentSet;
    }

    bool canInclude = true;
    for (int vertex : newIndependentSet) {
        if (graph.isAdjacentToSet(currentVertex, newIndependentSet)) {
            canInclude = false;
            break;
        }
    }

    set<int> independentSetWithoutCurrent;
    set<int> independentSetWithCurrent;

#pragma omp task shared(graph, newIndependentSet, currentVertex) firstprivate(independentSetWithoutCurrent)
    {
        independentSetWithoutCurrent = parallelMIS(graph, newIndependentSet, currentVertex + 1);
    }

    if (canInclude) {
        newIndependentSet.insert(currentVertex);
    }

#pragma omp task shared(graph, newIndependentSet, currentVertex) firstprivate(independentSetWithCurrent)
    {
        independentSetWithCurrent = parallelMIS(graph, newIndependentSet, currentVertex + 1);
    }

#pragma omp taskwait

    if (independentSetWithCurrent.size() > independentSetWithoutCurrent.size()) {
        return independentSetWithCurrent;
    } else {
        return independentSetWithoutCurrent;
    }
}

int main() {
    // ... Same as before ...
    // int procs;
    // procs=omp_get_num_procs();
    // cout<<"Setting threads :"<<(procs/2);
    // omp_set_num_threads(procs/2);


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

    set<int> maximalIndependentSet;

#pragma omp parallel
    {
#pragma omp single
        {
            maximalIndependentSet = parallelMIS(graph, set<int>(), 0);
        }
    }

    auto end = chrono::high_resolution_clock::now();
    cout<<endl;

    cout << "Maximal Independent Set (Parallel Backtracking): ";
    for (int v : maximalIndependentSet) {
        cout << v << " ";
    }
    cout << endl;

    chrono::duration<double, std::milli> duration = end - start;
    cout << "Time taken: " << fixed << setprecision(2) << duration.count() << " milliseconds" << endl;
    cout<<endl;

    return 0;
}
