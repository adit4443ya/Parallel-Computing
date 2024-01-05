#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <algorithm> // for std::sort
#include <iomanip>
#include <iterator>
#include <random>

using namespace std;
using json = nlohmann::json;
class Vertex
{
public:
    int id = 0;
    int randomV = 0;
    bool isActive = true;
    Vertex(int i)
    {
        id = i;
    }
    vector<int> neighbours;
    void addNeighbors(int i)
    {
        neighbours.push_back(i);
    }
};
class Graph
{
public:
    Graph(int numVertices);
    void addEdge(int u, int v);
    bool isAdjacentToSet(int vertex, const set<int> &independentSet) const;
    int getNumVertices() const;
    int getDegree(int vertex) const;
    const vector<int> &getAdjacencyList(int vertex) const;
    set<int> findMaximalIndependentSets();
    bool isIndependentSet(set<int> &independentset) const;

private:
    int numVertices;
    vector<vector<int>> adjacencyList;
    vector<vector<int>> adjacencyMatrix;
    vector<Vertex *> vertices;
};

Graph::Graph(int numVertices) : numVertices(numVertices)
{
    adjacencyList.resize(numVertices);
    for (int i = 0; i < numVertices; i++)
    {
        adjacencyMatrix.push_back(vector<int>());
        for (int j = 0; j < numVertices; j++)
        {
            adjacencyMatrix[i].push_back(0);
        }
    }
    for (int i = 0; i < numVertices; i++)
    {
        vertices.push_back(new Vertex(i));
    }
}

void Graph::addEdge(int u, int v)
{
    adjacencyList[u].push_back(v);
    adjacencyList[v].push_back(u);
    adjacencyMatrix[u][v] = 1;
    adjacencyMatrix[v][u] = 1;
    vertices[u]->addNeighbors(v);
    vertices[v]->addNeighbors(u);
}

bool Graph::isAdjacentToSet(int vertex, const set<int> &independentSet) const
{
    for (int v : independentSet)
    {
        // if (adjacencyList[vertex].count(v))
        // {
        //     return true;
        // }
        for (int j = 0; j < adjacencyList[v].size(); j++)
        {
            if (adjacencyList[v][j] == vertex)
            {
                return true;
            }
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

const vector<int> &Graph::getAdjacencyList(int vertex) const
{
    return adjacencyList[vertex];
}

// set<int> Graph::findMaximalIndependentSets()
// {
//     // vector<Vertex *> activeVer;
//     // for (int i = 0; i < numVertices; i++)
//     // {
//     //     activeVer.push_back(vertices[i]);
//     // }
//     // set<int> maximalIndependentSet;
//     // while (!activeVer.empty())
//     // {
//     //     cout << "ko";
//     //     unsigned seed = 42;
//     //     std::mt19937 gen(seed);
//     //     // Shuffle the objects to assign unique random values to value2 member
//     //     std::vector<int> randomValues(activeVer.size());
//     //     for (int i = 0; i < activeVer.size(); ++i)
//     //     {
//     //         randomValues[i] = i + 1;
//     //     }
//     //     std::shuffle(randomValues.begin(), randomValues.end(), gen);

//     //     for (int i = 0; i < activeVer.size(); ++i)
//     //     {
//     //         activeVer[i]->randomV = randomValues[i]; // Assigning unique random values to value2 member
//     //         cout<<activeVer[i]->id<<" ";
//     //     }cout<<endl;
//     //     cout<<activeVer.size()<<endl;
//     //     cout<<maximalIndependentSet.size()<<endl;
//     //     cout<<endl;
//     //     // set<int> m;
//     //     bool x = true;
//     //     for (Vertex *v : activeVer)
//     //     {
//     //         // x = true;
//     //         // cout << "hi";
//     //         // for (int u : v->neighbours)
//     //         // {
//     //         //     cout << "x";
//     //         //     if (v->randomV > vertices[u]->randomV)
//     //         //     {
//     //         //         x = false;
//     //         //         break;
//     //         //     }
//     //         // }
//     //         // if (x)
//     //         // {cout<<"2";
//     //             maximalIndependentSet.insert(v->id);
//     //             for (int u : v->neighbours)
//     //             {
//     //                 auto it = std::find_if(activeVer.begin(), activeVer.end(), [u](const Vertex *obj)
//     //                                        { return obj->id == u; });
//     //                 cout<<" d "<<endl;
//     //                 activeVer.erase(it);
//     //             }int l=v->id;
//     //             auto it = std::find_if(activeVer.begin(), activeVer.end(), [l](const Vertex *obj)
//     //                                        { return obj->id == l; });
//     //                 activeVer.erase(it);
//     //             break;
//     //         // }
//     //     }
//     // }
//     // cout<<"finsi"<<endl;
//     return maximalIndependentSet;
// }
set<int> Graph::findMaximalIndependentSets()
{
    vector<pair<int, long int>> verticesWithRandomValues;
    verticesWithRandomValues.resize(numVertices);
    vector<long int> uniqueNumbers;
    for (long int i = 0; i < numVertices * 20; i++)
    {
        uniqueNumbers.push_back(i);
    }
    random_shuffle(uniqueNumbers.begin(), uniqueNumbers.end());
    set<int> activeNodes;
    for (int i = 0; i < numVertices; ++i)
    {
        verticesWithRandomValues.emplace_back(i, uniqueNumbers[i]);
        activeNodes.insert(i);
    }
    bool p = true;
    set<int> result;
    while (!activeNodes.empty())
    {
        for (int i : activeNodes)
        {
            p = true;
            for (int j = 0; j < adjacencyList[i].size(); j++)
            {
                if (verticesWithRandomValues[adjacencyList[i][j]].second < verticesWithRandomValues[i].second)
                {
                    p = false;
                }
            }
            if (p)
            {
                result.insert(i);
                for (int j = 0; j < adjacencyList[i].size(); j++)
                {
                    activeNodes.erase(adjacencyList[i][j]);
                }
                activeNodes.erase(i);
                break;
            }
        }
    }
    return result;
}
bool Graph::isIndependentSet(set<int> &independentset) const
{
    for (int v : independentset)
    {
        for (int u : independentset)
        {
            if (v != u && isAdjacentToSet(v, independentset))
            {
                return false;
            }
        }
    }
    return true;
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

    set<int> maximalIndependentSet = graph.findMaximalIndependentSets();

    auto end = chrono::high_resolution_clock::now();
    cout << endl;
    cout << "Maximal Independent Sets (Luby's Algorithm):" << endl;
    cout << "{ ";
    for (int v : maximalIndependentSet)
    {
        cout << v << " ";
    }
    cout << "}\n";
    cout << graph.isIndependentSet(maximalIndependentSet) << endl;

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;
    cout << "Number of Nodes: " << numVertices << endl;
    return 0;
}
