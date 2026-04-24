#pragma once
#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <Core/Constants.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <functional>
#include <string>

namespace UI::Widgets{
class Button {
private:
    AppContext&           ctx;
    sf::Text              text;
    RoundedRectangleShape shape;

    sf::Color idleColor;
    sf::Color pressedColor;
    sf::Color hoverColor;
    sf::Color textColor;
    sf::Color outlineColor;
    sf::Color hoverOutlineColor;
    
    bool isHovered  = false;
    bool canPressed = false;
    bool isPressed  = false;

    std::function<void(sf::RenderTarget&, sf::RenderStates, sf::Color, sf::Vector2f)> iconDrawFunc;
    bool hasIcon = false;

public:
    Button(AppContext& context, const std::string& label, 
           sf::Vector2f pos, 
           sf::Vector2f size = {Config::UI::BUTTON_WIDTH, Config::UI::BUTTON_HEIGHT});

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    bool isCurrentlyPressed() const;

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setColors(sf::Color idle, sf::Color hover, sf::Color pressed, sf::Color textCol);
    void setLabel(const std::string& label);

    void update(sf::Vector2i mousePos);
    bool isClicked(const sf::Event& event);
    void draw();
    void animateClick();

    template <typename T>
    void setIcon(const T& iconShape) {
        T iconCopy = iconShape; 
        hasIcon = true;
        
        iconDrawFunc = [iconCopy](sf::RenderTarget& target, sf::RenderStates states, sf::Color color, sf::Vector2f pos) mutable {
            iconCopy.setFillColor(color);  
            iconCopy.setPosition(pos);     
            target.draw(iconCopy, states); 
        };
        
        text.setString(""); 
    }

private:
    void centerText();
    bool isAnimated = false;
    sf::Clock animationClock;
};
}
