#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include "Core/AppContext.hpp"
#include "Core/Constants.hpp"

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

    // Getters
    sf::Vector2f getPosition() const;
    float getRadius() const;
    std::string getLabel() const;
    bool contains(sf::Vector2f point) const;
    
    // UI Interactions
    void onHover();
    void onIdle();

    void draw();
};