#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <iomanip>   // for setprecision
#include <algorithm> // for std::max
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
    vector<pair<int, int>> getVerticesByDegree() ;
    vector<pair<int, int>> verticesByDegree; // {vertex, degree}
private:
    int numVertices;
    vector<set<int>> adjacencyList;
    vector<vector<int>> adjacencyMatrix;
};

Graph::Graph(int numVertices) : numVertices(numVertices) {
    adjacencyList.resize(numVertices);
    verticesByDegree.resize(numVertices);
    for (int i = 0; i < numVertices; i++) {
        verticesByDegree[i] = {i, 0};
    }
    for(int i=0;i<numVertices;i++){
        adjacencyMatrix.push_back(vector<int> ());
        for(int j=0;j<numVertices;j++){
            adjacencyMatrix[i].push_back(0);
        }
    }
}


void Graph::addEdge(int u, int v) {
    adjacencyList[u].insert(v);
    adjacencyList[v].insert(u);
    verticesByDegree[u].second++;
    // cout<<"ji";
    verticesByDegree[v].second++;
    adjacencyMatrix[u][v]=1;
    adjacencyMatrix[v][u]=1;
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

vector<pair<int, int>> Graph::getVerticesByDegree()  {
    sort(verticesByDegree.begin(), verticesByDegree.end(), [](const pair<int, int>& a, const pair<int, int>& b) {
        return a.second > b.second;
    });
    return verticesByDegree;
}
set<int> branchAndBoundMIS(const Graph &graph, set<int> currentSet, int currentVertex) {
    if (currentVertex == graph.getNumVertices()) {
        return currentSet;
    }

    int nextVertex = graph.verticesByDegree[currentVertex].first;

    if (!graph.isAdjacentToSet(nextVertex, currentSet)) {
        set<int> withCurrent = currentSet;
        withCurrent.insert(nextVertex);
        set<int> withCurrentResult;
        set<int> withoutCurrentResult;
        
        #pragma omp task shared (graph,withCurrentResult,withCurrent)
         withCurrentResult = branchAndBoundMIS(graph, withCurrent, currentVertex + 1);
        #pragma omp task shared (graph,withoutCurrentResult,currentSet)
         withoutCurrentResult = branchAndBoundMIS(graph, currentSet, currentVertex + 1);
        #pragma omp taskwait
        if (withCurrentResult.size() > withoutCurrentResult.size()) {
            return withCurrentResult;
        }
        return withoutCurrentResult;
    }

    return branchAndBoundMIS(graph, currentSet, currentVertex + 1);
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

int main()
{
    ifstream inputFile("random_graphP.json");
    if (!inputFile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }
  
    json jsonData;
    inputFile >> jsonData;

    int numVertices = jsonData["num_vertices"];
    Graph graph(numVertices);
    // #pragma omp parallel for
    
    for ( auto &edge : jsonData["edges"])
    {
        graph.addEdge(edge[0], edge[1]);

    }
    
graph.getVerticesByDegree();

    inputFile.close();
    set<int> maximalIndependentSet;
    auto start = chrono::high_resolution_clock::now();
    omp_set_num_threads(16);

    #pragma omp parallel
    {
        #pragma omp single
    maximalIndependentSet = branchAndBoundMIS(graph, set<int>(), 0);
    }
    auto end = chrono::high_resolution_clock::now();
    cout << endl;
  if (isIndependentSet(graph, maximalIndependentSet)) {
        cout << "The set is an independent set." << endl;
    } else {
        cout << "The set is NOT an independent set." << endl;
    }
    cout << "Maximal Independent Set (Branch and Bound with LDF): ";
    for (int v : maximalIndependentSet)
    {
        cout << v << " ";
    }
    cout << endl;

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;
    cout<<"Number of Vertices: "<<maximalIndependentSet.size()<<endl;
    cout <<"Number of Nodes: "<<numVertices<< endl;
    return 0;
}
