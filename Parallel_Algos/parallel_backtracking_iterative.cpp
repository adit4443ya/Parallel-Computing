#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <stack>
#include <omp.h>
#include <mutex>
#include <condition_variable>
#include <pthread.h>
#include <thread>
// #include
// #include "hashtable.h"

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


set<int> parallelIterativeBacktrackMIS(const Graph& graph) {
    int numVertices = graph.getNumVertices();
    set<int> maximalIndependentSet;
    int numthreads=16;
 omp_set_num_threads(numthreads);
    stack<pair<set<int>, int>> stk;
    stk.push(make_pair(set<int>(), 0));
    long long int o=0;
#pragma omp parallel shared(stk, maximalIndependentSet,o)
    {
//         stack<pair<set<int>, int>> stk;

// #pragma omp critical
//         localStk = stk;
#pragma omp single
        while (!stk.empty()) {o++;
// cout<<"ko";
            set<int> currentSet = stk.top().first;
            int currentVertex = stk.top().second;
            stk.pop();

            if (currentVertex == numVertices) {
#pragma omp critical
                {
                    if (currentSet.size() > maximalIndependentSet.size()) {
                        maximalIndependentSet = currentSet;
                    }
                }
                continue;
            }

            if (!graph.isAdjacentToSet(currentVertex, currentSet)) {
                set<int> withCurrent = currentSet;
                withCurrent.insert(currentVertex);
#pragma omp critical
                stk.push(make_pair(withCurrent, currentVertex + 1));
            }

            set<int> withoutCurrent = currentSet;
#pragma omp critical
            stk.push(make_pair(withoutCurrent, currentVertex + 1));
        }
    }

    cout<<"iterations->"<<o<<endl;
    return maximalIndependentSet;
}

int main() {
    ifstream inputFile("random_graphS.json");
    if (!inputFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return 1;
    }
cout<<"ji";
  
    json jsonData;
    inputFile >> jsonData;

    int numVertices = jsonData["num_vertices"];
    Graph graph(numVertices);

    for (const auto& edge : jsonData["edges"]) {
        graph.addEdge(edge[0], edge[1]);
    }

    inputFile.close();
    auto start = chrono::high_resolution_clock::now();
    set<int> maximalIndependentSet = parallelIterativeBacktrackMIS(graph);
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
