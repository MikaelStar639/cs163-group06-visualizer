#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include <Core/AppContext.hpp>
#include <Core/Constants.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>

namespace UI::Widgets {
    
class InputBar {
private:
    AppContext& ctx;

    RoundedRectangleShape box;
    sf::Text text;
    sf::Text placeholderText;

    sf::Color idleColor;
    sf::Color focusedColor;
    sf::Color outlineColor;
    sf::Color focusedOutlineColor;
    sf::Color textColor;
    sf::Color placeholderColor;

    std::string content;
    std::size_t maxLength = 20;
    bool isFocused = false;
    sf::RectangleShape cursor;
    bool showCursor = true;
    sf::Clock cursorClock;
    float cursorBlinkTime = 0.5f;

    void updateTextPositions();

public:
    InputBar(   AppContext& context,
                sf::Vector2f pos,
                sf::Vector2f size = {Config::UI::INPUT_BAR_WIDTH,
                                     Config::UI::INPUT_BAR_HEIGHT},
                const std::string& placeholder = "Enter here...");

    
    void handleEvent(const sf::Event& event);
    void update();
    void draw();

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setPlaceholder(const std::string& placeholder);
    void setText(const std::string& value);
    void setMaxLength(std::size_t length);

    const std::string& getText() const;
    bool empty() const;
    bool focused() const;

    void clear();
    void setFocus(bool focus);

    //return true if user pressed Enter while focus
    bool isSubmitted(const sf::Event& event) const;
};

}