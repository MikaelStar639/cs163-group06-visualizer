#include "UI/Widgets/InputBar.hpp"


namespace UI::Widgets {

InputBar::InputBar( AppContext& context,
                    sf::Vector2f pos,
                    sf::Vector2f size,
                    const std::string& placeholder)
    :   ctx(context),
        box(size, Config::UI::BUTTON_CORNER_RADIUS),
        text(ctx.font, "", Config::UI::FONT_SIZE_BUTTON),
        placeholderText(ctx.font, placeholder, Config::UI::FONT_SIZE_BUTTON),
        idleColor(Config::UI::Colors::InputBarIdle),
        focusedColor(Config::UI::Colors::InputBarFocuse),
        outlineColor(Config::UI::Colors::InputBarOutline),
        focusedOutlineColor(Config::UI::Colors::InputBarFocusedOutline),
        textColor(Config::UI::Colors::InputBarText),
        placeholderColor(Config::UI::Colors::InputBarPlaceholder)
{
    box.setPosition(pos);
    box.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    box.setFillColor(idleColor);
    box.setOutlineColor(outlineColor);

    text.setFillColor(textColor);
    placeholderText.setFillColor(placeholderColor);

    cursor.setSize({2.f, text.getCharacterSize() + 5.f});
    cursor.setFillColor(sf::Color::White);

    updateTextPositions();
}

void InputBar::updateTextPositions() {
    sf::Vector2f pos    = box.getPosition();
    sf::Vector2f size   = box.getSize();

    float paddingX      = 15.f;
    float centerY       = pos.y + size.y / 2.f;

    sf::FloatRect textBounds            = text.getLocalBounds();
    text.setOrigin({
        0.f,
        textBounds.position.y + textBounds.size.y / 2.f
    });
    text.setPosition({pos.x + paddingX, centerY});

    sf::FloatRect placeholderBounds     = placeholderText.getLocalBounds();
    placeholderText.setOrigin({
        0.f,
        placeholderBounds.position.y + placeholderBounds.size.y / 2.f
    });
    placeholderText.setPosition({pos.x + paddingX, centerY});

    

    float cursorX;
    

    if (content.empty()) {
        cursorX = placeholderText.getPosition().x;
    } else {
        sf::FloatRect global = text.getGlobalBounds();
        cursorX = global.position.x + global.size.x + 2.f;
    }

    float cursorY = box.getPosition().y + box.getSize().y / 2.f - cursor.getSize().y / 2.f;

    cursor.setPosition({cursorX, cursorY});
}

void InputBar::handleEvent(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()){
        if (mousePressed->button == sf::Mouse::Button::Left){
            sf::Vector2f mousePos(
                static_cast<float>(mousePressed->position.x),
                static_cast<float>(mousePressed->position.y)
            );

            isFocused = box.getGlobalBounds().contains(mousePos);
        }
    }
    if (!isFocused) return;

    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        char32_t unicode = textEntered->unicode;

        if (unicode == 8)  {
            if (!content.empty()) {
                content.pop_back();
                text.setString(content);
                updateTextPositions();
                cursorClock.restart();
                showCursor = true;
            }
        }
        else if (unicode >= 32 && unicode <= 126) {
            if (content.size() < maxLength) {
                content.push_back(static_cast<char> (unicode));
                text.setString(content);
                updateTextPositions();
                cursorClock.restart();
                showCursor = true;
            }
        }
        
    }
}

void InputBar::update() {
    if (isFocused) {
        if (cursorClock.getElapsedTime().asSeconds() >= cursorBlinkTime ) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
        box.setFillColor(focusedColor);
        box.setOutlineColor(focusedOutlineColor);
    } else {
        showCursor = false;
        box.setFillColor(idleColor);
        box.setOutlineColor(outlineColor);
    }
}

void InputBar::draw(){
    ctx.window.draw(box);

    if (content.empty()) {
        ctx.window.draw(placeholderText);
    } else {
        ctx.window.draw(text);
    }

    if (isFocused && showCursor) {
        ctx.window.draw(cursor);
    }
}

void InputBar::setPosition(sf::Vector2f pos) {
    box.setPosition(pos);
    updateTextPositions();
}

void InputBar::setSize(sf::Vector2f size) {
    box.setSize(size);
    updateTextPositions();
}

void InputBar::setPlaceholder(const std::string& placeholder) {
    placeholderText.setString(placeholder);
    updateTextPositions();
}

void InputBar::setText(const std::string& value) {
    content = value;
    text.setString(content);
    updateTextPositions();
}

void InputBar::setMaxLength(std::size_t length) {
    maxLength = length;
}

const std::string& InputBar::getText() const {
    return content;
}

bool InputBar::empty() const {
    return content.empty();
}

bool InputBar::focused() const {
    return isFocused;
}

void InputBar::clear() {
    content.clear();
    text.setString(content);
    updateTextPositions();
}

void InputBar::setFocus(bool focus) {
    isFocused = focus;
}

bool InputBar::isSubmitted(const sf::Event& event) const{
    if (!isFocused) return false;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        return keyPressed->code == sf::Keyboard::Key::Enter;
    }
    return false;
}

}