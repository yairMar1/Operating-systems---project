#ifndef MST_HPP
#define MST_HPP

#include "Graph.hpp"
#include "vector"

using namespace std;

class MST
{
public:
    virtual vector<pair<int, pair<int, int>>> computeMST(const Graph &graph) = 0;
    virtual ~MST() = default;
};

#endif // MST_HPP
