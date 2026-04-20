#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Core/AppContext.hpp"
#include "Core/Constants.hpp"

namespace UI::DSA{
class Node {
private:
    AppContext& ctx;
    sf::CircleShape shape;
    sf::Text text;
    std::string label;

    void centerText();

public:
    Node(AppContext& context, const std::string& label, sf::Vector2f pos);

    // Setters
    void setPosition(sf::Vector2f pos);
    void setLabel(const std::string& label);
    void setFillColor(sf::Color color);
    void setOutlineColor(sf::Color color);
    void setLabelColor(sf::Color color);
    void setScale(float scaleFactor);
    void setOutlineThickness(float thickness);

    // Getters
    sf::Vector2f getPosition() const;
    sf::Color getFillColor() const;
    sf::Color getOutlineColor() const;
    sf::Color getLabelColor() const;
    const std::string& getLabel() const;
    float getRadius() const;
    float getScale() const;

    bool contains(sf::Vector2f point) const;
    
    // UI Interactions
    void onHover();
    void onIdle();

    void draw();
};
}