#pragma once
#include <vector>
#include <utility>

using namespace std;

struct Edge
{
    int src, dest, weight;
};

class Graph
{
private:
    int V;
    vector<vector<pair<int, int>>> adjList;
    vector<int> parent;
    void dfs(int v, vector<bool> &visited);

public:
    Graph(int vertices);
    
    // graph operations
    void addEdge(int u, int v, int weight);
    void removeEdge(int u, int v);
    void addVertex(int newVertex);
    void removeVertex(int vertexToRemove);
    
    // information about the graph
    vector<pair<int, pair<int, int>>> getEdges() const;
    void buildSpanningTree(int root);
    vector<int> getPath(int v) const;
    int getNumVertices() const;
    vector<pair<int, int>> getAdjacentEdges(int v) const;
};
