#include "MSTServer.hpp"
#include <algorithm>
#include <numeric>
#include <queue>
#include <limits>

MSTServer::MSTServer(int num_threads) 
    : strategyFactory(make_unique<ConcreteStrategyFactory>()),
      threadPool(make_unique<ThreadPoll>(num_threads)) {}

void MSTServer::setGraph(int clientId, const Graph &newGraph) {
    clientGraphs[clientId] = make_unique<Graph>(newGraph);
}

void MSTServer::updateGraph(int clientId, const vector<pair<int, pair<int, int>>> &changes) {
    if (clientGraphs.find(clientId) == clientGraphs.end()) {
        throw runtime_error("Client graph not found");
    }

    for (const auto &change : changes) {
        int weight = change.first;
        int u = change.second.first;
        int v = change.second.second;
        clientGraphs[clientId]->addEdge(u, v, weight);
    }
}

void MSTServer::solveMST(int clientId, const std::string &strategyName) {
    if (clientGraphs.find(clientId) == clientGraphs.end()) {
        throw runtime_error("Client graph not found");
    }

    auto strategy = strategyFactory->createStrategy(strategyName);
    clientMSTResults[clientId] = strategy->computeMST(*clientGraphs[clientId]);
}

void MSTServer::calculateMeasurements(int clientId) {
    calculateTotalWeight(clientId);
    calculateLongestDistance(clientId);
    calculateAverageDistance(clientId);
    calculateShortestMSTDistance(clientId);
}

void MSTServer::visualizeGraph(int clientId) const {
    if (clientGraphs.find(clientId) == clientGraphs.end()) {
        throw runtime_error("Client graph not found");
    }

    GraphVisualizer visualizer(clientGraphs.at(clientId).get(), nullptr);
    visualizer.run();
}

void MSTServer::visualizeMST(int clientId) const {
    if (clientGraphs.find(clientId) == clientGraphs.end() || clientMSTResults.find(clientId) == clientMSTResults.end()) {
        throw runtime_error("Client graph or MST result not found");
    }

    GraphVisualizer visualizer(clientGraphs.at(clientId).get(), &clientMSTResults.at(clientId));
    visualizer.run();
}

void MSTServer::calculateTotalWeight(int clientId) {
    if (clientMSTResults.find(clientId) == clientMSTResults.end()) {
        throw runtime_error("MST result not found");
    }

    totalWeight[clientId] = std::accumulate(clientMSTResults[clientId].begin(), clientMSTResults[clientId].end(), 0,
        [](int sum, const auto &edge) { return sum + edge.first; });
}

void MSTServer::calculateLongestDistance(int clientId) {
    if (clientMSTResults.find(clientId) == clientMSTResults.end()) {
        throw runtime_error("MST result not found");
    }

    const auto &mst = clientMSTResults[clientId];
    int V = clientGraphs[clientId]->getNumVertices();

    // ניצור גרף המייצג את ה-MST
    vector<vector<pair<int, int>>> adjList(V);
    for (const auto &edge : mst) {
        int u = edge.second.first;
        int v = edge.second.second;
        int weight = edge.first;
        adjList[u].emplace_back(v, weight);
        adjList[v].emplace_back(u, weight);
    }

    // פונקציה למציאת הקודקוד הרחוק ביותר מ-start
    auto findFurthestNode = [&](int start) {
        vector<int> dist(V, numeric_limits<int>::max());
        dist[start] = 0;
        priority_queue<pair<int, int>, vector<pair<int, int>>, greater<>> pq;
        pq.push({0, start});
        
        int furthestNode = start;
        int maxDistance = 0;

        while (!pq.empty()) {
            int u = pq.top().second;
            int d = pq.top().first;
            pq.pop();

            if (d > dist[u]) continue;

            for (const auto &edge : adjList[u]) {
                int v = edge.first;
                int weight = edge.second;
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    pq.push({dist[v], v});
                    if (dist[v] > maxDistance) {
                        maxDistance = dist[v];
                        furthestNode = v;
                    }
                }
            }
        }
        return make_pair(furthestNode, maxDistance);
    };

    // שלב 1: מצא את הקודקוד הרחוק ביותר מנקודה כלשהי (כאן בחרנו קודקוד 0)
    auto [nodeA, _dummy] = findFurthestNode(0);  // שינוי מזהה מ- _ ל- _dummy

    // שלב 2: בצע DFS שני מהקודקוד שנמצא כדי למצוא את המרחק הרחוק ביותר
    auto [dummyNode, longestDist] = findFurthestNode(nodeA);  // שינוי מזהה מ- _ ל- dummyNode

    // שמור את התוצאה
    longestDistance[clientId] = longestDist;
}

// This function calculates the average of the edges from each vertex to each vertex.
// We used the BFS algorithm to reach from each vertex to each vertex,
// And also so that we do not repeat the calculation twice (from X to Y and from Y to X).
void MSTServer::calculateAverageDistance(int clientId) {
    if (clientMSTResults.find(clientId) == clientMSTResults.end()) {
        throw runtime_error("MST result not found");
    }

    const auto &mst = clientMSTResults[clientId];
    int V = clientGraphs[clientId]->getNumVertices();

    // נבנה רשימת שכנות של ה-MST
    vector<vector<pair<int, int>>> adjList(V);
    for (const auto &edge : mst) {
        int u = edge.second.first;
        int v = edge.second.second;
        int weight = edge.first;
        adjList[u].emplace_back(v, weight);
        adjList[v].emplace_back(u, weight);
    }

    // משתנה לשמירת סכום כל המרחקים
    int totalDistance = 0;
    int pairCount = 0; // סופר את כל הזוגות שנבדקו

    // פונקציה לחישוב המרחקים מכל קודקוד בעזרת BFS
    auto calculateDistancesFromNode = [&](int start) {
        vector<int> dist(V, numeric_limits<int>::max());
        dist[start] = 0;
        queue<int> q;
        q.push(start);

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (const auto &neighbor : adjList[u]) {
                int v = neighbor.first;
                int weight = neighbor.second;
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    q.push(v);
                }
            }
        }

        // נוסיף את כל המרחקים מ-start לכל שאר הקודקודים
        for (int i = 0; i < V; ++i) {
            if (i != start && dist[i] != numeric_limits<int>::max()) {
                totalDistance += dist[i];
                ++pairCount;
            }
        }
    };

    // נבצע את החישוב עבור כל קודקוד
    for (int i = 0; i < V; ++i) {
        calculateDistancesFromNode(i);
    }

    // חישוב הממוצע
    if (pairCount == 0) {
        throw runtime_error("No valid pairs found in MST");
    }

    averageDistance[clientId] = static_cast<double>(totalDistance) / pairCount;
}

void MSTServer::calculateShortestMSTDistance(int clientId) {
    if (clientMSTResults.find(clientId) == clientMSTResults.end()) {
        throw runtime_error("MST result not found");
    }

    const auto &mst = clientMSTResults[clientId];
    shortestMSTDistance[clientId] = min_element(mst.begin(), mst.end(),
        [](const auto &a, const auto &b) { return a.first < b.first; })->first;
}