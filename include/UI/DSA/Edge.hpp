#pragma once
#include <SFML/Graphics.hpp>
#include "Core/AppContext.hpp"
#include "Core/Constants.hpp"
#include "Node.hpp"

namespace UI::DSA {

class Edge {
public:
    Edge(Node* src, Node* dest, AppContext& context, float weight = 1.0f, float thickness = 2.0f);

    void update();

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

    Node* source;
    Node* dest;

    float weight;
    sf::Text weightText;

    sf::ConvexShape lineShape;
    sf::Color color;
    float thickness;

    sf::ConvexShape arrowhead;
    bool isDirected; // You can toggle this in the constructor
};

} // namespace UI::DSA