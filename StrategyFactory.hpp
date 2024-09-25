#ifndef STRATEGY_FACTORY_HPP
#define STRATEGY_FACTORY_HPP

#include "MST.hpp"
#include "Graph.hpp"
#include <memory>
#include <string>

using namespace std;

//This class directs the construction of the MST according to the user's request
class StrategyFactory{
public:
    virtual unique_ptr<MST> createStrategy(const string &strategyName) = 0;
    virtual ~StrategyFactory() = default;
};

class ConcreteStrategyFactory : public StrategyFactory{
public:
    unique_ptr<MST> createStrategy(const string &strategyName) override;
};

//This class creates the MST using Kruskal's algorithm
class KruskalMST : public MST{
public:
    vector<pair<int, pair<int, int>>> computeMST(const Graph &graph) override;
};

//This class creates the MST using Prim's algorithm
class PrimMST : public MST{
public:
    vector<pair<int, pair<int, int>>> computeMST(const Graph &graph) override;
};

#endif // STRATEGY_FACTORY_H
