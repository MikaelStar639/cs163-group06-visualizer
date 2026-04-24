#include "UI/Widgets/Button.hpp"

namespace UI::Widgets{
Button::Button(AppContext& context, const std::string& label, 
    sf::Vector2f pos, sf::Vector2f size):
    ctx(context), 
    shape(size, Config::UI::BUTTON_CORNER_RADIUS),
    text(ctx.font, label, Config::UI::FONT_SIZE_BUTTON),
    idleColor(Config::UI::Colors::ButtonIdle),
    hoverColor(Config::UI::Colors::ButtonHover),
    textColor(Config::UI::Colors::ButtonText),
    pressedColor(Config::UI::Colors::ButtonPressed),
    outlineColor(Config::UI::Colors::ButtonOutline),
    hoverOutlineColor(Config::UI::Colors::ButtonOutlineHover)
{
    shape.setPosition        (pos);
    shape.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    shape.setFillColor       (Config::UI::Colors::ButtonIdle);
    shape.setOutlineColor    (Config::UI::Colors::ButtonOutline);
    text. setFillColor       (Config::UI::Colors::ButtonText);

    centerText();
}

void Button::setColors(sf::Color idle, sf::Color hover, sf::Color pressed, sf::Color textCol) {
    idleColor = idle;
    hoverColor = hover;
    pressedColor = pressed;
    textColor = textCol;
    
    shape.setFillColor(idleColor);
    text.setFillColor(textColor);
}

void Button::setSize(sf::Vector2f size) {
    shape.setSize(size);
    centerText(); 
}

void Button::setPosition(sf::Vector2f pos) {
    shape.setPosition(pos);
    centerText(); 
}

void Button::setLabel(const std::string& label) {
    text.setString(label);
    centerText();
}

void Button::centerText() {
    sf::FloatRect bounds = text.getLocalBounds();
    text.setOrigin({
        bounds.position.x + bounds.size.x / 2.f, 
        bounds.position.y + bounds.size.y / 2.f
    });
    
    sf::Vector2f shapePos = shape.getPosition();
    sf::Vector2f shapeSize = shape.getSize();
    text.setPosition({shapePos.x + shapeSize.x / 2.f, shapePos.y + shapeSize.y / 2.f});
}

void Button::update(sf::Vector2i mousePos) {
    isHovered = shape.getGlobalBounds().contains(static_cast<sf::Vector2f>(mousePos));
    
    bool mouseHeld = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    bool shouldAnimate = isAnimated && animationClock.getElapsedTime().asSeconds() < 0.1f;

    if (shouldAnimate) {
        shape.setOutlineColor(hoverOutlineColor);
        shape.setFillColor(pressedColor);
        return;
    } else {
        if (isAnimated) isAnimated = false;
    }

    if (isHovered) {
        shape.setOutlineColor(hoverOutlineColor); 
        
        if (mouseHeld && isPressed) {
            shape.setFillColor(pressedColor);
        } else {
            isPressed = false; 
            shape.setFillColor(hoverColor);
        }
    } else {
        isPressed = false; 
        shape.setOutlineColor(outlineColor); 
        shape.setFillColor(idleColor);
    }
}

bool Button::isClicked(const sf::Event& event) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left && isHovered) {
            isPressed = true; 
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            if (isHovered && isPressed) {
                isPressed = false;
                return true; 
            }
            isPressed = false;
        }
    }
    return false;
}

sf::Vector2f Button::getPosition() const {
    return shape.getPosition();
}

sf::Vector2f Button::getSize() const {
    return shape.getSize();
}

void Button::animateClick() {
    isAnimated = true;
    animationClock.restart();
}

bool Button::isCurrentlyPressed() const {
    return isPressed || (isAnimated && animationClock.getElapsedTime().asSeconds() < 0.1f);
}

void Button::draw() {
    ctx.window.draw(shape);
    
    bool shouldDrawPressed = isPressed || (isAnimated && animationClock.getElapsedTime().asSeconds() < 0.1f);
    sf::Vector2f offset = shouldDrawPressed ? sf::Vector2f(0.f, 2.f) : sf::Vector2f(0.f, 0.f);

    if (hasIcon && iconDrawFunc) {
        sf::Vector2f shapePos = shape.getPosition();
        sf::Vector2f shapeSize = shape.getSize();
        sf::Vector2f center(shapePos.x + shapeSize.x / 2.f, shapePos.y + shapeSize.y / 2.f);
        
        iconDrawFunc(ctx.window, sf::RenderStates::Default, text.getFillColor(), center + offset);
    } else {
        if (shouldDrawPressed) {
            text.move({0.f, 2.f});
            ctx.window.draw(text);
            text.move({0.f, -2.f}); 
        } else {
            ctx.window.draw(text);
        }
    }
}
}