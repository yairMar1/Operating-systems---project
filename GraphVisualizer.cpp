#include "GraphVisualizer.hpp"
#include <cmath>
#include <sstream>
#include <iostream>
#include <SFML/System.hpp>

GraphVisualizer::GraphVisualizer(const Graph *g, const std::vector<std::pair<int, std::pair<int, int>>> *m): window(sf::VideoMode(800, 600), "Graph Visualizer"), graph(g), mst(m){
    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")){
        throw std::runtime_error("Failed to load font");
    }
    createVertices();
    createEdgesAndWeights();
}

void GraphVisualizer::createVertices(){
    int numVertices = graph->getNumVertices();
    float radius = 20.0f;
    float centerX = 400.0f;
    float centerY = 300.0f;
    float angle = 2 * M_PI / numVertices;

    for (int i = 0; i < numVertices; ++i){
        sf::CircleShape vertex(radius);
        vertex.setFillColor(sf::Color::White);
        vertex.setOutlineThickness(2);
        vertex.setOutlineColor(sf::Color::Black);
        float x = centerX + 250 * std::cos(i * angle);
        float y = centerY + 250 * std::sin(i * angle);
        vertex.setPosition(x - radius, y - radius);
        vertices.push_back(vertex);

        sf::Text label(std::to_string(i), font, 20);
        label.setFillColor(sf::Color::Black);
        label.setPosition(x - 10, y - 15);
        vertexLabels.push_back(label);
    }
}

void GraphVisualizer::createEdgesAndWeights(){
    int numVertices = graph->getNumVertices();
    for (int i = 0; i < numVertices; ++i){
        for (const auto &edge : graph->getAdjacentEdges(i)){
            int j = edge.first;
            if (i < j){
                sf::Vertex start(sf::Vector2f(vertices[i].getPosition().x + 20, vertices[i].getPosition().y + 20));
                sf::Vertex end(sf::Vector2f(vertices[j].getPosition().x + 20, vertices[j].getPosition().y + 20));
                start.color = sf::Color::Black;
                end.color = sf::Color::Black;
                edges.push_back(start);
                edges.push_back(end);

                sf::Text weight(std::to_string(edge.second), font, 15);
                weight.setFillColor(sf::Color::Red);
                weight.setPosition((start.position.x + end.position.x) / 2, (start.position.y + end.position.y) / 2);
                edgeWeights.push_back(weight);
            }
        }
    }
}

void GraphVisualizer::run(){
    int frameCount = 0;
    sf::Clock clock;
    while (window.isOpen()){
        sf::Time elapsed = clock.restart();
        sf::Event event;
        while (window.pollEvent(event)){
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::White);

        frameCount++;
        // Remove the frame count print

        for (size_t i = 0; i < edges.size(); i += 2){
            sf::Vertex line[] = {edges[i], edges[i + 1]};
            window.draw(line, 2, sf::Lines);
        }

        for (const auto &vertex : vertices){
            window.draw(vertex);
        }

        for (const auto &label : vertexLabels){
            window.draw(label);
        }

        for (const auto &weight : edgeWeights){
            window.draw(weight);
        }

        if (mst){
            for (const auto &edge : *mst){
                int u = edge.second.first;
                int v = edge.second.second;
                sf::Vertex line[] = {
                    // Set the color of the MST edges to red
                    sf::Vertex(sf::Vector2f(vertices[u].getPosition().x + 20, vertices[u].getPosition().y + 20), sf::Color::Red),
                    sf::Vertex(sf::Vector2f(vertices[v].getPosition().x + 20, vertices[v].getPosition().y + 20), sf::Color::Red)};
                window.draw(line, 2, sf::Lines);
            }
        }

        window.display();

        sf::sleep(sf::milliseconds(16) - elapsed);
    }
}