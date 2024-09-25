#include "Graph.hpp"
#include <algorithm>
#include <limits>
#include <queue>
#include <stdexcept>

using namespace std;

Graph::Graph(int vertices) : V(vertices), adjList(vertices), parent(vertices, -1) {}

void Graph::addEdge(int u, int v, int weight){
    adjList[u].push_back({v, weight});
    adjList[v].push_back({u, weight});
}

vector<pair<int, pair<int, int>>> Graph::getEdges() const{
    vector<pair<int, pair<int, int>>> edges;
    for (int u = 0; u < V; ++u)
    {
        for (const auto &edge : adjList[u])
        {
            int v = edge.first;
            int weight = edge.second;
            if (u < v)
                edges.push_back({weight, {u, v}});
        }
    }
    return edges;
}

void Graph::buildSpanningTree(int root){
    vector<bool> visited(V, false);
    dfs(root, visited);
}

void Graph::dfs(int v, vector<bool> &visited){
    visited[v] = true;
    for (const auto &edge : adjList[v])
    {
        int u = edge.first;
        if (!visited[u])
        {
            parent[u] = v;
            dfs(u, visited);
        }
    }
}

vector<int> Graph::getPath(int v) const{
    vector<int> path;
    while (v != -1)
    {
        path.push_back(v);
        v = parent[v];
    }
    reverse(path.begin(), path.end());
    return path;
}

int Graph::getNumVertices() const{
    return V;
}

vector<pair<int, int>> Graph::getAdjacentEdges(int v) const{
    return adjList[v];
}

void Graph::removeEdge(int u, int v) {
    // Remove edge from u's adjacency list
    auto &uEdges = adjList[u];
    uEdges.erase(remove_if(uEdges.begin(), uEdges.end(),
                           [v](const pair<int, int> &edge) { return edge.first == v; }),
                 uEdges.end());

    // Remove edge from v's adjacency list
    auto &vEdges = adjList[v];
    vEdges.erase(remove_if(vEdges.begin(), vEdges.end(),
                           [u](const pair<int, int> &edge) { return edge.first == u; }),
                 vEdges.end());
}

void Graph::addVertex(int newVertex) {
    if (newVertex >= V) {
        // Increase the size of the adjacency list and parent vector
        adjList.resize(newVertex + 1);
        parent.resize(newVertex + 1, -1);
        V = newVertex + 1; // Update the number of vertices
    }
}

void Graph::removeVertex(int vertexToRemove) {
    if (vertexToRemove >= V || vertexToRemove < 0) {
        throw runtime_error("Vertex index out of bounds");
    }

    // Remove all edges connected to the vertex
    for (int i = 0; i < V; ++i) {
        if (i == vertexToRemove) continue;
        auto &edges = adjList[i];
        edges.erase(std::remove_if(edges.begin(), edges.end(),
                                   [vertexToRemove](const std::pair<int, int> &edge) { return edge.first == vertexToRemove; }),
                    edges.end());
    }

    // Remove the vertex itself
    adjList.erase(adjList.begin() + vertexToRemove);
    parent.erase(parent.begin() + vertexToRemove);
    V--; // Update the number of vertices

    // Update adjacency lists to reflect the new vertex indices
    for (auto &edges : adjList) {
        for (auto &edge : edges) {
            if (edge.first > vertexToRemove) {
                edge.first--;
            }
        }
    }
}