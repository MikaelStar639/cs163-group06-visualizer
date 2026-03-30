#pragma once
#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <Core/Constants.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <vector>
#include <string>

class Dropdown {
private:
    AppContext&           ctx;
    sf::Text              mainText;
    RoundedRectangleShape mainBox;
    RoundedRectangleShape arrowBox;
    sf::ConvexShape       arrowTriangle;

    sf::Color idleColor;
    sf::Color pressedColor;
    sf::Color hoverColor;
    sf::Color textColor;
    sf::Color outlineColor;
    sf::Color hoverOutlineColor;
    
    bool isHovered  = false;
    bool isDropped  = false;
    int selectedIndex = -1;

    std::vector<RoundedRectangleShape> itemBoxes;
    std::vector<sf::Text>              itemTexts;
    std::vector<std::string>           options;

public:
    Dropdown(AppContext& context, const std::string& label, 
             sf::Vector2f pos, 
             sf::Vector2f size = {Config::UI::BUTTON_WIDTH, Config::UI::BUTTON_HEIGHT});

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setColors(sf::Color idle, sf::Color hover, sf::Color pressed, sf::Color textCol);
    void setLabel(const std::string& label);
    void setOptions(const std::vector<std::string>& opts);

    void update(sf::Vector2i mousePos);
    bool isClicked(const sf::Event& event);
    void draw();

    bool getIsDropped() const;
    int getSelectedIndex() const;
    std::string getSelectedText() const;

private:
    void updateLayout();
};
