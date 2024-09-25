#ifndef GRAPH_VISUALIZER_HPP
#define GRAPH_VISUALIZER_HPP

#include <SFML/Graphics.hpp>
#include "Graph.hpp"
#include "MST.hpp"

using namespace std;

class GraphVisualizer
{
private:
    sf::RenderWindow window;
    const Graph *graph;
    const vector<pair<int, pair<int, int>>> *mst;
    vector<sf::CircleShape> vertices;
    vector<sf::Vertex> edges;
    vector<sf::Text> vertexLabels;
    vector<sf::Text> edgeWeights;
    sf::Font font;

    void createVertices();
    void createEdgesAndWeights();

public:
    GraphVisualizer(const Graph *g, const vector<pair<int, pair<int, int>>> *m = nullptr);
    void run();
};

#endif // GRAPH_VISUALIZER_HPP
