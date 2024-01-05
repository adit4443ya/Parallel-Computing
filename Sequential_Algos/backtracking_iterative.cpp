#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <initializer_list> // initializer_list

#include <chrono>
#include <stack>

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

set<int> iterativeBacktrackMIS(const Graph& graph) {
    int numVertices = graph.getNumVertices();
    set<int> maximalIndependentSet;

    stack<pair<set<int>, int>> stk;
    stk.push(make_pair(set<int>(), 0));

    while (!stk.empty()) {
        set<int> currentSet = stk.top().first;
        int currentVertex = stk.top().second;
        stk.pop();

        if (currentVertex == numVertices) {
            if (currentSet.size() > maximalIndependentSet.size()) {
                maximalIndependentSet = currentSet;
            }
            continue;
        }

        if (!graph.isAdjacentToSet(currentVertex, currentSet)) {
            set<int> withCurrent = currentSet;
            withCurrent.insert(currentVertex);
            stk.push(make_pair(withCurrent, currentVertex + 1));
        }

        set<int> withoutCurrent = currentSet;
        stk.push(make_pair(withoutCurrent, currentVertex + 1));
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

    auto start = chrono::high_resolution_clock::now();
    set<int> maximalIndependentSet = iterativeBacktrackMIS(graph);
    auto end = chrono::high_resolution_clock::now();

    cout << "Maximal Independent Set (Iterative Backtracking): ";
    for (int v : maximalIndependentSet) {
        cout << v << " ";
    }
    cout << endl;

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;

    return 0;
}
