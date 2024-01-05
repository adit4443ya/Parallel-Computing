#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include "json.hpp"
#include <chrono>
#include <algorithm> // for std::sort
#include <iomanip>
#include <iterator>

using namespace std;
using json = nlohmann::json;

// MIS Algorithm
std::set<std::set<int>> MIS(vector<vector<int>> mat, int num_nodes)
{
    std::set<std::set<int>> mis;
    std::set<int> I;
    int num_remain = num_nodes;
    std::set<int> nodes_remain;
    for (int i = 0; i < num_nodes; i++)
    {
        nodes_remain.insert(i);
    }
    int cur_node = 0;
    std::set<int> neighbors;
    I.insert(cur_node);

    while (!nodes_remain.empty())
    {
        while (num_remain > 0)
        {
            for (int i = 0; i < num_nodes; i++)
            {
                // removing current node's neighbors
                if ((mat[cur_node][i] > 0) && (nodes_remain.find(i) != nodes_remain.end()))
                {
                    num_remain--;
                    neighbors.insert(i);
                    nodes_remain.erase(i);
                }
            }
            nodes_remain.erase(cur_node);
            num_remain--;
            // choose a new current node, insert
            if (!nodes_remain.empty())
            {
                cur_node = *(nodes_remain.begin());
                I.insert(cur_node);
            }
        }
        mis.insert(I);
        I.clear();
        num_remain = neighbors.size();
        nodes_remain = neighbors;
        if (!nodes_remain.empty())
        {
            cur_node = *(nodes_remain.begin());
            I.insert(cur_node);
        }
        neighbors.clear();
    }

    return mis;
}

class Graph
{
public:
    Graph(int numVertices);
    void addEdge(int u, int v);
    bool isAdjacentToSet(int vertex, const set<int> &independentSet) const;
    int getNumVertices() const;
    int getDegree(int vertex) const;
    const set<int> &getAdjacencyList(int vertex) const;
    set<set<int>> findMaximalIndependentSets() ;
    bool isIndependentSet ( set<int>& independentset) const;
    void makeAdjacencyMatrix() ;
private:
    int numVertices;
    vector<set<int>> adjacencyList;
    vector<vector<int>> adjacencyMatrix;
};

Graph::Graph(int numVertices) : numVertices(numVertices)
{
    adjacencyList.resize(numVertices);
    for(int i=0;i<numVertices;i++){
        adjacencyMatrix.push_back(vector<int> ());
        for(int j=0;j<numVertices;j++){
            adjacencyMatrix[i].push_back(0);
        }
    }
}

void Graph::addEdge(int u, int v)
{
    adjacencyList[u].insert(v);
    adjacencyList[v].insert(u);
    adjacencyMatrix[u][v]=1;
    adjacencyMatrix[v][u]=1;
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

set<set<int>> Graph::findMaximalIndependentSets() 
{
    set<set<int>> maximalIndependentSets = MIS(this->adjacencyMatrix, numVertices);
    bool areAllSetsIndependent=true;
    for(set<int> i:maximalIndependentSets){
        if(!this->isIndependentSet(i)){
            areAllSetsIndependent=false;
        }
    }
    if(areAllSetsIndependent){
        cout<<"All sets are Independent"<<endl;
    }
    else{
        cout<<"Not all sets are Independent"<<endl;
    }
    return maximalIndependentSets;
}

bool Graph::isIndependentSet( set<int>& independentSet) const
{
    for (int v : independentSet) {
        for (int u : independentSet) {
            if (v != u && this->isAdjacentToSet(v, {u})) {
                return false;  // Not an independent set
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

    set<set<int>> maximalIndependentSets = graph.findMaximalIndependentSets();

    auto end = chrono::high_resolution_clock::now();
    cout << endl;
    int l = 0,k=0,max=0;
    cout << "Maximal Independent Sets (Converted from Adjacency List):" << endl;
    for (const auto &independentSet : maximalIndependentSets)
    {k=0;
        cout << "{ ";
        for (int v : independentSet)
        {k++;
            cout << v << " ";
        }
        if(max<k){
            max=k;
        }
        cout << "}\n";
        l++;
    }

    chrono::duration<double> duration = end - start;
    double milliseconds = duration.count() * 1000.0;

    cout << "Time taken: " << fixed << setprecision(2) << milliseconds << " milliseconds" << endl;
    cout <<"Number of Nodes: "<<numVertices<< endl;
    cout<<"Maximum Cardinality of Set: "<<max<<endl;
    cout << "Number of Sets: " << l << endl;
    return 0;
}
