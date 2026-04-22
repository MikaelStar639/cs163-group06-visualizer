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

    // Setters
    void setColor(sf::Color color);
    void setWeight(std::string newWeight);
    void setThickness(float newThickness);
    void toggleDirection(bool directed);
    void flipDirection();
    void setNodes(Node* src, Node* dest);
    
    // Getters
    std::string getWeight() const;
    sf::Color   getColor() const;
    Node* getSource() const { return source; }
    Node* getDest() const { return dest; }
    float getThickness() const { return thickness; }
    bool connectsTo(const Node* node) const;


private:
    AppContext& ctx; // Reference to access the font

    Node* source;
    Node* dest;

    std::string weight;
    sf::Text weightText;

    sf::ConvexShape lineShape;
    sf::Color color;
    float thickness;

    sf::ConvexShape arrowHead;
    bool isDirected; 
};

} 
// namespace UI::DSA