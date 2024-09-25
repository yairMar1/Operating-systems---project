#include "StrategyFactory.hpp"
#include <algorithm>
#include <queue>
#include <stdexcept>
#include <limits>
#include <vector>
#include <functional>

using namespace std;

unique_ptr<MST> ConcreteStrategyFactory::createStrategy(const string &strategyName){
    if (strategyName == "kruskal")
    {
        return make_unique<KruskalMST>();
    }
    else if (strategyName == "prim")
    {
        return make_unique<PrimMST>();
    }
    throw runtime_error("Unknown strategy");
}

// Kruskal's algorithm implementation
vector<pair<int, pair<int, int>>> KruskalMST::computeMST(const Graph &graph){
    vector<pair<int, pair<int, int>>> result;
    vector<pair<int, pair<int, int>>> edges = graph.getEdges();
    sort(edges.begin(), edges.end());

    int V = graph.getNumVertices();
    vector<int> parent(V);
    vector<int> rank(V, 0);

    for (int i = 0; i < V; i++)
        parent[i] = i;

    function<int(int)> find = [&parent, &find](int i) -> int
    {
        if (parent[i] != i)
            parent[i] = find(parent[i]);
        return parent[i];
    };

    auto unite = [&parent, &rank, &find](int x, int y)
    {
        int xroot = find(x);
        int yroot = find(y);

        if (rank[xroot] < rank[yroot])
            parent[xroot] = yroot;
        else if (rank[xroot] > rank[yroot])
            parent[yroot] = xroot;
        else
        {
            parent[yroot] = xroot;
            rank[xroot]++;
        }
    };

    for (const auto &edge : edges)
    {
        int u = edge.second.first;
        int v = edge.second.second;

        int set_u = find(u);
        int set_v = find(v);

        if (set_u != set_v)
        {
            result.push_back(edge);
            unite(set_u, set_v);
        }
    }

    return result;
}

// Prim's algorithm implementation
vector<pair<int, pair<int, int>>> PrimMST::computeMST(const Graph &graph){
    vector<pair<int, pair<int, int>>> result;
    int V = graph.getNumVertices();
    vector<bool> visited(V, false);
    vector<int> key(V, numeric_limits<int>::max());
    vector<int> parent(V, -1);

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;

    int start = 0;
    pq.push({0, start});
    key[start] = 0;

    while (!pq.empty())
    {
        int u = pq.top().second;
        pq.pop();

        if (visited[u])
            continue;

        visited[u] = true;

        if (parent[u] != -1)
        {
            result.push_back({key[u], {parent[u], u}});
        }

        for (const auto &edge : graph.getAdjacentEdges(u))
        {
            int v = edge.first;
            int weight = edge.second;

            if (!visited[v] && weight < key[v])
            {
                parent[v] = u;
                key[v] = weight;
                pq.push({key[v], v});
            }
        }
    }

    return result;
}
