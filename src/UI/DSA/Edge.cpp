#include "UI/DSA/Edge.hpp"
#include <iostream>
#include <cmath>

namespace UI::DSA {

// Basic connection
Edge::Edge(AppContext& context, Node* src, Node* dest)
    : Edge(context, src, dest, false, "", Config::UI::EDGE_THICKNESS, Config::UI::Colors::EdgeFill) {}

// Connection with Weight
Edge::Edge(AppContext& context, Node* src, Node* dest, const std::string& weightStr)
    : Edge(context, src, dest, false, weightStr, Config::UI::EDGE_THICKNESS, Config::UI::Colors::EdgeFill) {}

// Connection with Directionality
Edge::Edge(AppContext& context, Node* src, Node* dest, bool directed)
    : Edge(context, src, dest, directed, "", Config::UI::EDGE_THICKNESS, Config::UI::Colors::EdgeFill) {}

// Connection with Weight and Directionality
Edge::Edge(AppContext& context, Node* src, Node* dest, bool directed, const std::string& weightStr)
    : Edge(context, src, dest, directed, weightStr, Config::UI::EDGE_THICKNESS, Config::UI::Colors::EdgeFill) {}

// Everything
Edge::Edge(AppContext& context, Node* src, Node* dest, 
           bool directed, const std::string& weightStr, float newThickness, sf::Color newColor)
    : ctx(context), source(src), dest(dest), isDirected(directed),
      weight(weightStr), thickness(newThickness), color(newColor), weightText(ctx.font) 
{
    // Color Setup
    lineShape.setFillColor(color);
    arrowHead.setFillColor(color);
    
    // Geometry Setup
    lineShape.setPointCount(4); 
    arrowHead.setPointCount(3); 

    // Local coordinate of the arrowHead
    float arrowLength = thickness * 5.0f; 
    float arrowWidth  = thickness * 4.0f;
    arrowHead.setPoint(0, {0.f, 0.f});                        
    arrowHead.setPoint(1, {-arrowLength, -arrowWidth / 2.f});  
    arrowHead.setPoint(2, {-arrowLength, arrowWidth / 2.f});   

    // Setup Text
    weightText.setCharacterSize(Config::UI::FONT_SIZE_NODE * 0.8f);
    weightText.setFillColor(sf::Color::White);
    setWeight(weightStr); 
}


void Edge::setWeight(std::string newWeight) {
    weight = newWeight;
    // Use std::to_string or a precision formatter
    weightText.setString(weight);
    
    // Center the text origin so it stays balanced on the line
    sf::FloatRect bounds = weightText.getLocalBounds();
    weightText.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
}

void Edge::update() {
    if (!source || !dest) return; 

    sf::Vector2f p1 = source->getPosition();
    sf::Vector2f p2 = dest->getPosition();

    sf::Vector2f direction = p2 - p1;
    float length = std::sqrt(direction.x * direction.x + direction.y * direction.y);
    
    //early return 
    if (length < 1e-5f) return; 

    sf::Vector2f unitDir = direction / length;
    sf::Vector2f unitPerp(-unitDir.y, unitDir.x);

    sf::Vector2f offset = unitPerp * (thickness / 2.f);

    lineShape.setPoint(0, p1 - offset);
    lineShape.setPoint(1, p1 + offset);
    lineShape.setPoint(2, p2 + offset);
    lineShape.setPoint(3, p2 - offset);

    if (!weight.empty()) {
        sf::Vector2f midPoint = (p1 + p2) / 2.f;
        sf::Vector2f normal(-direction.y / length, direction.x / length);
        float offsetDistance = thickness * 8.f;
        weightText.setPosition(midPoint + (normal * offsetDistance));
    }

    if (isDirected) {
        float radius = dest->getRadius();
        sf::Vector2f arrowTip = p2 - (unitDir * radius);
        
        arrowHead.setPosition(arrowTip);
        float angleInRadians = std::atan2(direction.y, direction.x);
        arrowHead.setRotation(sf::radians(angleInRadians));
    }
}

void Edge::draw() {
    ctx.window.draw(lineShape);
    if (isDirected) {
        ctx.window.draw(arrowHead);
    }
    if (!weight.empty()){
        ctx.window.draw(weightText);
    }
}

void Edge::setColor(sf::Color newColor) {
    color = newColor;
    lineShape.setFillColor(color);
    arrowHead.setFillColor(color);
}

void Edge::setThickness(float newThickness) {
    thickness = newThickness;
    
    float arrowLength = thickness * 5.0f; 
    float arrowWidth  = thickness * 4.0f;
    arrowHead.setPoint(1, {-arrowLength, -arrowWidth / 2.f});  
    arrowHead.setPoint(2, {-arrowLength, arrowWidth / 2.f});   
}

void Edge::toggleDirection(bool directed) {
    this->isDirected = directed;
    
    // Ensure the arrowHead matches the current line color
    if (isDirected) {
        arrowHead.setFillColor(this->color);
    }
}

void Edge::flipDirection(){
    std::swap(source, dest);
}

void Edge::setNodes(Node* src, Node* dest) {
    this->source = src;
    this->dest = dest;
}

std::string Edge::getWeight() const{
    return weight;
}

sf::Color Edge::getColor() const{
    return color;
}

bool Edge::connectsTo(const Node* node) const {
    return source == node || dest == node;
}

} // namespace UI::DSA