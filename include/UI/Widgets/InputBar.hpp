#pragma once

#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include <tuple>

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

    bool dirty = false;

    // cursor
    bool isFocused = false;
    sf::RectangleShape cursor;
    bool showCursor = true;
    sf::Clock cursorClock;
    float cursorBlinkTime = 0.5f;

    // validate
    InputType type = InputType::AnyText;
    bool isValid = true;
    std::string errorMessage;

    bool isCharacterAllowed(char32_t unicode) const;
    bool validateContent();
    bool isMultiline() const;

    void fitTextToBox(sf::Text& target, float horizontalPadding = 15.f, float verticalPadding = 10.f);
    void updateTextPositions();

    // to move cursor

    std::size_t caretPos = 0;
    int preferredColumn = -1;

    std::size_t getLineStart(std::size_t pos) const;
    std::size_t getLineEnd(std::size_t pos) const;
    int getLineOfPos(std::size_t pos) const;
    int getColumnOfPos(std::size_t pos) const;
    std::size_t getPosFromLineColumn(int line, int column) const;

    float getLineHeight() const;
    float measureTextWidth(const std::string& s) const;

    void moveCaretLeft();
    void moveCaretRight();
    void moveCaretUp();
    void moveCaretDown();
    void moveCaretHome();
    void moveCaretEnd();
    void moveCaretToMouse(sf::Vector2f mousePos);

public:
    InputBar(AppContext& context,
             sf::Vector2f pos,
             sf::Vector2f size = {Config::UI::INPUT_BAR_WIDTH, Config::UI::INPUT_BAR_HEIGHT},
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

    void setType(InputType inputType);
    bool empty() const;
    bool focused() const;
    bool valid() const;
    const std::string& getErrorMessage() const;
    const std::string& getText() const;

    void clear();
    void setFocus(bool focus);
    bool isSubmitted(const sf::Event& event) const;

    // mới
    bool consumeChanged();
    std::vector<std::string> getLines(bool skipEmpty = true) const;

    bool parseGraphData(int nodeCount,
                        std::vector<int>& nodeValues,
                        std::vector<std::tuple<int, int, int>>& edges,
                        std::string& outError) const;
};

}