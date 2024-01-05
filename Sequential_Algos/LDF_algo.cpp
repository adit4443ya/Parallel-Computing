#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <json.hpp>
#include <chrono>
#include <iomanip>   // for setprecision
#include <algorithm> // for std::max

using namespace std;
using json = nlohmann::json;

class Graph
{
public:
    Graph(int numVertices);
    void addEdge(int u, int v);
    int getNumVertices() const;
    const set<int> &getAdjacencyList(int vertex) const;
    int getDegree(int vertex) const;

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

int Graph::getNumVertices() const
{
    return numVertices;
}

const set<int> &Graph::getAdjacencyList(int vertex) const
{
    return adjacencyList[vertex];
}

int Graph::getDegree(int vertex) const
{
    return adjacencyList[vertex].size();
}

set<int> largestDegreeFirstMIS(const Graph &graph)
{
    set<int> maximalIndependentSet;
    vector<int> degrees(graph.getNumVertices());

    for (int v = 0; v < graph.getNumVertices(); ++v)
    {
        degrees[v] = graph.getDegree(v);
    }

    while (!degrees.empty())
    {
        int maxDegreeVertex = max_element(degrees.begin(), degrees.end()) - degrees.begin();
        maximalIndependentSet.insert(maxDegreeVertex);

        for (int neighbor : graph.getAdjacencyList(maxDegreeVertex))
        {
            degrees[neighbor] = -1; // Mark neighbors as visited
        }

        degrees[maxDegreeVertex] = -1;                                            // Mark the selected vertex as visited
        degrees.erase(remove(degrees.begin(), degrees.end(), -1), degrees.end()); // Remove visited vertices
    }

    return maximalIndependentSet;
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
    set<int> maximalIndependentSet = largestDegreeFirstMIS(graph);
    auto end = chrono::high_resolution_clock::now();
    cout << endl;

    cout << "Maximal Independent Set (Largest Degree First): ";
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
