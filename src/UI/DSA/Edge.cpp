#include "UI/DSA/Edge.hpp"
#include <cmath>

namespace UI::DSA {

Edge::Edge(Node* src, Node* dest, AppContext& context, float weight, float thickness)
    : source(src), dest(dest), ctx(context), weight(weight), thickness(thickness), weightText(ctx.font) 
{
    color = sf::Color(150, 150, 150); // Default grey
    isDirected = 0;
    // Initialize Weight Text
    weightText.setCharacterSize(Config::UI::FONT_SIZE_NODE * 0.8f); // Slightly smaller than node text
    weightText.setFillColor(sf::Color::White);
    setWeight(weight); // Set initial string
}

void Edge::setWeight(float newWeight) {
    weight = newWeight;
    // Use std::to_string or a precision formatter
    weightText.setString(std::to_string(static_cast<int>(weight)));
    
    // Center the text origin so it stays balanced on the line
    sf::FloatRect bounds = weightText.getLocalBounds();
    weightText.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });
}

void Edge::update() {

    sf::Vector2f p1 = source->getPosition();
    sf::Vector2f p2 = dest->getPosition();

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

    sf::Vector2f midPoint = (p1 + p2) / 2.f;

    if (length > 0) {
        // Find the unit normal vector (perpendicular to the line)
        // Rotating (x, y) by 90 degrees gives (-y, x)
        sf::Vector2f normal(-direction.y / length, direction.x / length);

        // Adjust this value to change how far "above" the line the text sits
        float offsetDistance = 15.f; 
        
        // Final position: Midpoint + (Normal * Offset)
        weightText.setPosition(midPoint + (normal * offsetDistance));
        
        // Optional: Rotate the text to match the line angle!
        // float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;
        // weightText.setRotation(sf::degrees(angle)); 
    }

    if (length > 0) {
        sf::Vector2f unitDir = direction / length;

        // 1. Calculate Position (Same as before)
        float radius = dest->getRadius();
        sf::Vector2f arrowTip = p2 - (unitDir * radius);

        // 2. Setup the Triangle with DYNAMIC Scaling
        // Adjust these multipliers to get the "look" you like
        float arrowLength = thickness * 5.0f; 
        float arrowWidth  = thickness * 4.0f;

        arrowhead.setPointCount(3);
        arrowhead.setPoint(0, {0.f, 0.f});                         // The tip
        arrowhead.setPoint(1, {-arrowLength, -arrowWidth / 2.f});  // Back-left
        arrowhead.setPoint(2, {-arrowLength, arrowWidth / 2.f});   // Back-right

        arrowhead.setFillColor(color);
        arrowhead.setPosition(arrowTip);

        // 3. Rotation (Same as before)
        float angle = std::atan2(direction.y, direction.x) * 180.f / 3.14159f;
        arrowhead.setRotation(sf::degrees(angle));
    }
}

void Edge::draw(sf::RenderTarget& target) {
    target.draw(lineShape);
    if (isDirected) {
        target.draw(arrowhead);
    }
    target.draw(weightText);
}

void Edge::setColor(sf::Color newColor) {
    color = newColor;
    lineShape.setFillColor(color);
}

void Edge::setThickness(float newThickness) {
    thickness = newThickness;
}
void Edge::toggleDirection(bool directed) {
    this->isDirected = directed;
    
    // Ensure the arrowhead matches the current line color
    if (isDirected) {
        arrowhead.setFillColor(this->color);
    }
}

void Edge::flipDirection(){
    std::swap(source, dest);
}

float Edge::getWeight() const{
    return weight;
}
} // namespace UI::DSA