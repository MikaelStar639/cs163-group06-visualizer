#pragma once

#include <SFML/Graphics.hpp>
#include <string>

#include <Core/AppContext.hpp>
#include <Core/Constants.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>



namespace UI::Widgets {

enum class InputType {
    AnyText,
    Integer,
    IntegerList,
    Word,
    EdgeTriple
};
    
class InputBar {
private:
    AppContext& ctx;

    RoundedRectangleShape box;
    sf::Text text;
    sf::Text placeholderText;
    sf::Text errorText;

    sf::Color idleColor;
    sf::Color focusedColor;
    sf::Color outlineColor;
    sf::Color focusedOutlineColor;
    sf::Color textColor;
    sf::Color placeholderColor;
    sf::Color invalidColor;
    sf::Color invalidOutlineColor;
    sf::Color errorColor;

    std::string content;

    std::size_t maxLength = 20;

    // for cursor
    bool isFocused = false;
    sf::RectangleShape cursor;
    bool showCursor = true;
    sf::Clock cursorClock;
    float cursorBlinkTime = 0.5f;

    //for validate input data
    InputType type = InputType::AnyText;
    bool isValid = true;
    std::string errorMessage;
    bool isCharacterAllowed(char32_t unicode) const;
    bool validateContent();
    
    void fitTextToBox(sf::Text& target, float horizontalPadding = 15.f, float verticalPadding = 10.f);
    void updateTextPositions();

public:
    InputBar(   AppContext& context,
                sf::Vector2f pos,
                sf::Vector2f size = {Config::UI::INPUT_BAR_WIDTH,
                                     Config::UI::INPUT_BAR_HEIGHT},
                const std::string& placeholder = "Enter here...",
                InputType inputType = InputType::AnyText);

    
    void handleEvent(const sf::Event& event);
    void update();
    void draw();

    void setPosition(sf::Vector2f pos);
    void setSize(sf::Vector2f size);
    void setPlaceholder(const std::string& placeholder);
    void setText(const std::string& value);
    void setMaxLength(std::size_t length);

    //for validate input data
    void setType(InputType inputType);
    bool empty() const;
    bool focused() const;
    bool valid() const;
    const std::string& getErrorMessage() const;


    const std::string& getText() const;
    
    void clear();
    void setFocus(bool focus);

    //return true if user pressed Enter while focus
    bool isSubmitted(const sf::Event& event) const;
};

}