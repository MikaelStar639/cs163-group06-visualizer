#include "UI/DSA/Node.hpp"

namespace UI::DSA{

Node::Node(AppContext& context, const std::string& label, sf::Vector2f pos)
    : ctx(context), label(label), text(ctx.font, label, Config::UI::FONT_SIZE_NODE) 
{
    float r = Config::UI::NODE_RADIUS;
    
    shape.setPointCount(Config::UI::NODE_POINT_COUNT);
    shape.setRadius(r);
    shape.setOutlineThickness(Config::UI::NODE_OUTLINE_THICKNESS);
    shape.setFillColor(Config::UI::Colors::NodeFill);
    shape.setOutlineColor(Config::UI::Colors::NodeOutline);
    
    shape.setOrigin({r, r}); 
    
    shape.setPosition(pos);
    
    text.setFillColor(Config::UI::Colors::NodeText);
    centerText();
}

void Node::centerText() {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({
        bounds.position.x + bounds.size.x / 2.f, 
        bounds.position.y + bounds.size.y / 2.f
    });
    
    text.setPosition(shape.getPosition());
}

void Node::setScale(float scaleFactor) {
    shape.setScale(sf::Vector2f(scaleFactor, scaleFactor));
    text.setScale(sf::Vector2f(scaleFactor, scaleFactor));
}

// --- Setters ---
void Node::setPosition(sf::Vector2f pos) { 
    shape.setPosition(pos); 
    centerText(); 
}

void Node::setLabel(const std::string& newLabel) { 
    label = newLabel; 
    text.setString(label); 
    centerText(); 
}

void Node::setFillColor(sf::Color color) { shape.setFillColor(color); }
void Node::setOutlineColor(sf::Color color) { shape.setOutlineColor(color); }
void Node::setLabelColor(sf::Color color) { text.setFillColor(color); }

// --- Getters ---
sf::Vector2f Node::getPosition() const { 
    return shape.getPosition(); 
}

float Node::getRadius() const { return shape.getRadius(); }
const std::string& Node::getLabel() const { return label; }
sf::Color Node::getFillColor() const { return shape.getFillColor(); }
sf::Color Node::getOutlineColor() const { return shape.getOutlineColor(); }
sf::Color Node::getLabelColor() const { return text.getFillColor(); }
float Node::getScale() const { return shape.getScale().x; }

bool Node::contains(sf::Vector2f point) const {
    float r = shape.getRadius();
    sf::Vector2f center = shape.getPosition(); 
    
    float dx = point.x - center.x;
    float dy = point.y - center.y;
    
    return (dx * dx + dy * dy <= r * r);
}

// --- UI Interactions ---
void Node::onHover() { 
    shape.setOutlineColor(Config::UI::Colors::NodeHover); 
}

void Node::onIdle() { 
    shape.setOutlineColor(Config::UI::Colors::NodeOutline); 
}

// --- Draw ---
void Node::draw() {
    ctx.window.draw(shape);
    ctx.window.draw(text);
}

} // namespace UI::DSA