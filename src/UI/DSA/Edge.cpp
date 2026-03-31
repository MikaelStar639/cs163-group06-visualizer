#include "UI/DSA/Edge.hpp"
#include <cmath>

namespace UI::DSA {

Edge::Edge(size_t srcIdx, size_t destIdx, float thickness)
    : sourceIdx(srcIdx), destIdx(destIdx), thickness(thickness) 
{
    color = sf::Color(150, 150, 150); // Default grey
}

void Edge::update(const std::vector<Node>& nodes) {
    // Safety check: ensure indices are still valid
    if (sourceIdx >= nodes.size() || destIdx >= nodes.size()) return;

    sf::Vector2f p1 = nodes[sourceIdx].getPosition();
    sf::Vector2f p2 = nodes[destIdx].getPosition();

    // Calculate direction vector and perpendicular vector for thickness
    sf::Vector2f direction = p2 - p1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    if (length != 0) {
        sf::Vector2f unitDir = direction / length;
        sf::Vector2f unitPerp(-unitDir.y, unitDir.x);

        sf::Vector2f offset = unitPerp * (thickness / 2.f);

        // Create a rectangle representing the thick line
        lineShape.setPointCount(4);
        lineShape.setPoint(0, p1 - offset);
        lineShape.setPoint(1, p1 + offset);
        lineShape.setPoint(2, p2 + offset);
        lineShape.setPoint(3, p2 - offset);
        lineShape.setFillColor(color);
    }
}

void Edge::draw(sf::RenderTarget& target) {
    target.draw(lineShape);
}

void Edge::setColor(sf::Color newColor) {
    color = newColor;
    lineShape.setFillColor(color);
}

} // namespace UI::DSA