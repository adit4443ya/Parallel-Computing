#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <algorithm> // for std::sort
#include <iomanip>

using namespace std;
using json = nlohmann::json;

class Graph
{
public:
    Graph(int numVertices);
    void addEdge(int u, int v);
    bool isAdjacentToSet(int vertex, const set<int> &independentSet) const;
    int getNumVertices() const;
    int getDegree(int vertex) const; // Added function
    const set<int> &getAdjacencyList(int vertex) const;

private:
    int numVertices;
    vector<set<int>> adjacencyList;
};

Graph::Graph(int numVertices) : numVertices(numVertices)
{
    adjacencyList.resize(numVertices);
}

void Graph::addEdge(int u, int v)
{
    adjacencyList[u].insert(v);
    adjacencyList[v].insert(u);
}

bool Graph::isAdjacentToSet(int vertex, const set<int> &independentSet) const
{
    for (int v : independentSet)
    {
        if (adjacencyList[vertex].count(v))
        {
            return true;
        }
    }
    return false;
}

int Graph::getNumVertices() const
{
    return numVertices;
}

int Graph::getDegree(int vertex) const
{
    return adjacencyList[vertex].size();
}

const set<int> &Graph::getAdjacencyList(int vertex) const
{
    return adjacencyList[vertex];
}

set<int> backtrackMIS(const Graph &graph, const set<int> &independentSet, int currentVertex)
{
    set<int> newIndependentSet = independentSet;

    if (currentVertex == graph.getNumVertices())
    {
        return newIndependentSet;
    }

    bool canInclude = true;
    for (int vertex : newIndependentSet)
    {
        if (graph.isAdjacentToSet(currentVertex, newIndependentSet))
        {
            canInclude = false;
            break;
        }
    }

    set<int> independentSetWithoutCurrent = backtrackMIS(graph, newIndependentSet, currentVertex + 1);

    if (canInclude)
    {
        newIndependentSet.insert(currentVertex);
    }

    set<int> independentSetWithCurrent = backtrackMIS(graph, newIndependentSet, currentVertex + 1);

    if (independentSetWithCurrent.size() > independentSetWithoutCurrent.size())
    {
        return independentSetWithCurrent;
    }
    else
    {
        return independentSetWithoutCurrent;
    }
}

bool compareByDegreeDesc(const pair<int, int> &a, const pair<int, int> &b)
{
    return a.second > b.second;
}

int main()
{
    ifstream inputFile("random_graphS.json");
    if (!inputFile.is_open())
    {
        cerr << "Error opening file!" << endl;
        return 1;
    }

    json jsonData;
    inputFile >> jsonData;

    int numVertices = jsonData["num_vertices"];
    Graph graph(numVertices);

    for (const auto &edge : jsonData["edges"])
    {
        graph.addEdge(edge[0], edge[1]);
    }

    inputFile.close();

    auto start = chrono::high_resolution_clock::now();

    set<int> maximalIndependentSet = backtrackMIS(graph, set<int>(), 0);

    auto end = chrono::high_resolution_clock::now();
    cout << endl;

    cout << "Maximal Independent Set (Backtracking - Largest Degree First): ";
    for (int v : maximalIndependentSet)
    {
        cout << v << " ";
    }
    cout << endl;

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;
    cout << endl;

    return 0;
}
