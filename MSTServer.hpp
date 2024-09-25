#ifndef MST_SERVER_HPP
#define MST_SERVER_HPP

#include "Graph.hpp"
#include "StrategyFactory.hpp"
#include "ThreadPoll.hpp"
#include <memory>
#include <vector>
#include <unordered_map>
#include "GraphVisualizer.hpp"

class MSTServer
{
public:
    unordered_map<int, unique_ptr<Graph>> clientGraphs;
    unique_ptr<StrategyFactory> strategyFactory;
    unique_ptr<ThreadPoll> threadPool;
    unordered_map<int, vector<pair<int, pair<int, int>>>> clientMSTResults;

public:
    MSTServer(int num_threads);
    void setGraph(int clientId, const Graph &newGraph);
    void updateGraph(int clientId, const vector<pair<int, pair<int, int>>> &changes);
    void solveMST(int clientId, const string &strategyName);
    void calculateMeasurements(int clientId);
    void visualizeGraph(int clientId) const;
    void visualizeMST(int clientId) const;

    // Measurement results
    unordered_map<int, int> totalWeight;
    unordered_map<int, int> longestDistance;
    unordered_map<int, double> averageDistance;
    unordered_map<int, int> shortestMSTDistance;

private:
    void calculateTotalWeight(int clientId);
    void calculateLongestDistance(int clientId);
    void calculateAverageDistance(int clientId);
    void calculateShortestMSTDistance(int clientId);
};

#endif // MST_SERVER_HPP