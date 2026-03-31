#pragma once
#include <SFML/Graphics.hpp>
#include "Node.hpp"

namespace UI::DSA {

class Edge {
public:
    // Pass indices instead of pointers
    Edge(size_t srcIdx, size_t destIdx, float thickness = 2.0f);

    // Now update() needs to know where the nodes are located
    void update(const std::vector<Node>& nodes);

    void draw(sf::RenderTarget& target);

    // Setters for visual states (useful for DSA animations)
    void setColor(sf::Color color);
    void setThickness(float thickness);

private:
    size_t sourceIdx;
    size_t destIdx;
    sf::ConvexShape lineShape;
    sf::Color color;
    float thickness;
};

} // namespace UI::DSA