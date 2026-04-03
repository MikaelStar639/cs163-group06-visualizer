#include "UI/Widgets/Dropdown.hpp"

namespace UI::Widgets {

Dropdown::Dropdown(AppContext& context, const std::string& label, 
    sf::Vector2f pos, sf::Vector2f size):
    ctx(context), 
    mainBox(size, Config::UI::BUTTON_CORNER_RADIUS),
    arrowBox(sf::Vector2f(size.y, size.y), Config::UI::BUTTON_CORNER_RADIUS),
    mainText(ctx.font, label, Config::UI::FONT_SIZE_BUTTON),
    idleColor(Config::UI::Colors::ButtonIdle),
    hoverColor(Config::UI::Colors::ButtonHover),
    textColor(Config::UI::Colors::ButtonText),
    pressedColor(Config::UI::Colors::ButtonPressed),
    outlineColor(Config::UI::Colors::ButtonOutline),
    hoverOutlineColor(Config::UI::Colors::ButtonOutlineHover)
{
    mainBox.setPosition        (pos);
    mainBox.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    mainBox.setFillColor       (idleColor);
    mainBox.setOutlineColor    (outlineColor);
    
    // Config arrowBox similar to button outline
    arrowBox.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    arrowBox.setFillColor       (hoverColor); 
    arrowBox.setOutlineColor    (outlineColor);

    arrowTriangle.setPointCount(3);
    arrowTriangle.setFillColor(textColor);
    
    mainText.setFillColor      (textColor);

    updateLayout();
}

void Dropdown::setColors(sf::Color idle, sf::Color hover, sf::Color pressed, sf::Color textCol) {
    idleColor = idle;
    hoverColor = hover;
    pressedColor = pressed;
    textColor = textCol;
    
    mainBox.setFillColor(idleColor);
    mainText.setFillColor(textColor);
    arrowBox.setFillColor(hoverColor);
    arrowTriangle.setFillColor(textColor);
}

void Dropdown::setSize(sf::Vector2f size) {
    mainBox.setSize(size);
    arrowBox.setSize({size.y, size.y});
    updateLayout(); 
}

void Dropdown::setPosition(sf::Vector2f pos) {
    mainBox.setPosition(pos);
    updateLayout(); 
}

void Dropdown::setLabel(const std::string& label) {
    mainText.setString(label);
    updateLayout();
}

void Dropdown::clearSelection() {
    selectedIndex = -1;
    isDropped = false;
}

void Dropdown::setSelectedIndex(int index) {
    if (index >= 0 && index < (int)options.size()) {
        selectedIndex = index;
        mainText.setString(options[index]);
    } else {
        selectedIndex = -1;
    }
    updateLayout();
}

void Dropdown::setOptions(const std::vector<std::string>& opts) {
    options = opts;
    itemBoxes.clear();
    itemTexts.clear();
    
    sf::Vector2f pos = mainBox.getPosition();
    sf::Vector2f size = mainBox.getSize();
    
    for (size_t i = 0; i < options.size(); ++i) {
        RoundedRectangleShape box(size, Config::UI::BUTTON_CORNER_RADIUS);
        box.setPosition(sf::Vector2f(pos.x, pos.y + size.y * (i + 1)));
        box.setFillColor(idleColor);
        box.setOutlineColor(outlineColor);
        box.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
        itemBoxes.push_back(box);

        sf::Text text(ctx.font, options[i], Config::UI::FONT_SIZE_BUTTON);
        text.setFillColor(textColor);
        
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({
            bounds.position.x + bounds.size.x / 2.f, 
            bounds.position.y + bounds.size.y / 2.f
        });
        text.setPosition({box.getPosition().x + size.x / 2.f, box.getPosition().y + size.y / 2.f});
        itemTexts.push_back(text);
    }
}

void Dropdown::updateLayout() {
    sf::Vector2f pos = mainBox.getPosition();
    sf::Vector2f size = mainBox.getSize();
    
    float arrowAreaWidth = size.y; 
    arrowBox.setPosition(sf::Vector2f(pos.x + size.x - arrowAreaWidth, pos.y));
    
    // Update Triangle
    arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
    arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
    arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));
    arrowTriangle.setPosition(sf::Vector2f(
        arrowBox.getPosition().x + (arrowAreaWidth - 16.f) / 2.f,
        arrowBox.getPosition().y + (size.y - 10.f) / 2.f
    ));

    sf::FloatRect bounds = mainText.getLocalBounds();
    mainText.setOrigin({
        bounds.position.x + bounds.size.x / 2.f, 
        bounds.position.y + bounds.size.y / 2.f
    });
    
    float mainTextX = pos.x + (size.x - arrowAreaWidth) / 2.f;
    mainText.setPosition({mainTextX, pos.y + size.y / 2.f});
    
    // Rebuild options layout
    if (!options.empty()) {
        for (size_t i = 0; i < options.size(); ++i) {
            itemBoxes[i].setSize(size);
            itemBoxes[i].setPosition(sf::Vector2f(pos.x, pos.y + size.y * (i + 1)));
            itemTexts[i].setPosition({itemBoxes[i].getPosition().x + size.x / 2.f, itemBoxes[i].getPosition().y + size.y / 2.f});
        }
    }
}

void Dropdown::update(sf::Vector2i mousePos) {
    sf::Vector2f mappedMouse = ctx.window.mapPixelToCoords(mousePos);
    
    isHovered = mainBox.getGlobalBounds().contains(mappedMouse);
    
    if (isDropped) {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 10.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 10.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 0.f));

        mainBox.setFillColor(pressedColor);
        arrowBox.setFillColor(idleColor);
        mainBox.setOutlineColor(hoverOutlineColor);
    } else if (isHovered) {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));

        mainBox.setFillColor(hoverColor);
        arrowBox.setFillColor(pressedColor);
        mainBox.setOutlineColor(hoverOutlineColor);
    } else {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));

        mainBox.setFillColor(idleColor);
        arrowBox.setFillColor(hoverColor);
        mainBox.setOutlineColor(outlineColor);
    }

    if (isDropped) {
        for (size_t i = 0; i < itemBoxes.size(); ++i) {
            if (itemBoxes[i].getGlobalBounds().contains(mappedMouse)) {
                itemBoxes[i].setFillColor(hoverColor);
            } else {
                itemBoxes[i].setFillColor(idleColor);
            }
        }
    }
}

bool Dropdown::isClicked(const sf::Event& event) {
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos = ctx.window.mapPixelToCoords(sf::Mouse::getPosition(ctx.window));

            bool clickedInsideMainBox = mainBox.getGlobalBounds().contains(mousePos);

            if (clickedInsideMainBox) {
                isDropped = !isDropped;
                return true;
            }

            if (isDropped) {
                bool clickedAnyItem = false;

                for (size_t i = 0; i < itemBoxes.size(); ++i) {
                    if (itemBoxes[i].getGlobalBounds().contains(mousePos)) {
                        selectedIndex = static_cast<int>(i);
                        isDropped = false;
                        clickedAnyItem = true;
                        // setLabel(options[selectedIndex]); // You can enable this if dropdown should change text automatically
                        return true;
                    }
                }

                if (!clickedAnyItem) {
                    isDropped = false; 
                }
            }
        }
    }
    return false;
}

void Dropdown::draw() {
    ctx.window.draw(mainBox);
    ctx.window.draw(arrowBox);
    ctx.window.draw(arrowTriangle);
    ctx.window.draw(mainText);

    if (isDropped) {
        for (const auto& box : itemBoxes) {
            ctx.window.draw(box);
        }
        for (const auto& text : itemTexts) {
            ctx.window.draw(text);
        }
    }
}

bool Dropdown::getIsDropped() const {
    return isDropped;
}

int Dropdown::getSelectedIndex() const {
    return selectedIndex;
}

std::string Dropdown::getSelectedText() const {
    if (selectedIndex >= 0 && selectedIndex < (int)options.size()) {
        return options[selectedIndex];
    }
    return "";
}

}
