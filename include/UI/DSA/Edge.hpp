#pragma once
#include <SFML/Graphics.hpp>
#include "Core/AppContext.hpp"
#include "Core/Constants.hpp"
#include "Node.hpp"

namespace UI::DSA {

class Edge {
public:
    // Pass indices instead of pointers
    Edge(size_t srcIdx, size_t destIdx, AppContext& context, float weight = 1.0f, float thickness = 2.0f);

    // Now update() needs to know where the nodes are located
    void update(const std::vector<Node>& nodes);

    void draw(sf::RenderTarget& target);

    // Setters for visual states (useful for DSA animations)
    void setColor(sf::Color color);
    void setThickness(float newThickness);

    void setWeight(float newWeight);
    float getWeight() const;

    void toggleDirection(bool directed);
    void flipDirection();
private:
    AppContext& ctx; // Reference to access the font

    size_t sourceIdx;
    size_t destIdx;

    float weight;
    sf::Text weightText;

    sf::ConvexShape lineShape;
    sf::Color color;
    float thickness;

    sf::ConvexShape arrowhead;
    bool isDirected; // You can toggle this in the constructor
};

} // namespace UI::DSA