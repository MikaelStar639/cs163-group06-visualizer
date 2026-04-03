#pragma once
#include <SFML/Graphics.hpp>
#include "Core/AppContext.hpp"
#include "Core/Constants.hpp"
#include "Node.hpp"

namespace UI::DSA {

class Edge {
public:
    // Basic connection
    Edge(AppContext& context, Node* src, Node* dest);

    // Connection with Weight
    Edge(AppContext& context, Node* src, Node* dest, const std::string& weightStr);
    
    // Connection with Directionality
    Edge(AppContext& context, Node* src, Node* dest, bool directed);
    
    // Connection with Weight and Directionality
    Edge(AppContext& context, Node* src, Node* dest, bool directed, const std::string& weightStr);

    // Everything
    Edge(AppContext& context, Node* src, Node* dest, 
         bool directed, const std::string& weightStr, float thickness, sf::Color color);

    void update();

    void draw();

    // Setters for visual states (useful for DSA animations)
    void setColor(sf::Color color);
    void setThickness(float newThickness);

    void setWeight(std::string newWeight);
    std::string getWeight() const;

    void toggleDirection(bool directed);
    void flipDirection();
private:
    AppContext& ctx; // Reference to access the font

    Node* source;
    Node* dest;

    std::string weight;
    sf::Text weightText;

    sf::ConvexShape lineShape;
    sf::Color color;
    float thickness;

    sf::ConvexShape arrowhead;
    bool isDirected; // You can toggle this in the constructor
};

} 
// namespace UI::DSA