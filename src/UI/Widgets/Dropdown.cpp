#include "UI/Widgets/Dropdown.hpp"

namespace UI::Widgets {

Dropdown::Dropdown(AppContext& context, const std::string& label, 
    sf::Vector2f pos, sf::Vector2f size):
    ctx(context), 
    mainBox(size, Config::UI::BUTTON_CORNER_RADIUS),
    arrowBox(sf::Vector2f(size.y, size.y), Config::UI::BUTTON_CORNER_RADIUS),
    mainText(ctx.font, label, Config::UI::FONT_SIZE_BUTTON),
    dropdownBg({size.x, 0.f}, Config::UI::BUTTON_CORNER_RADIUS),
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
    
    // Config arrowBox — outline matches the blue accent color
    arrowBox.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    arrowBox.setFillColor       (hoverColor); 
    arrowBox.setOutlineColor    (hoverOutlineColor);

    arrowTriangle.setPointCount(3);
    arrowTriangle.setFillColor(textColor);
    
    mainText.setFillColor      (textColor);

    // Dropdown background container
    dropdownBg.setFillColor(idleColor);
    dropdownBg.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    dropdownBg.setOutlineColor(outlineColor);

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
    dropdownBg.setFillColor(idleColor);
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
    dividers.clear();
    
    sf::Vector2f pos = mainBox.getPosition();
    sf::Vector2f size = mainBox.getSize();
    
    float dropGap = 4.f;       // gap between mainBox and dropdown container
    float padding = 6.f;       // padding inside the container
    float itemGap = 4.f;       // gap between items
    float itemHeight = size.y - 4.f; // slightly shorter than the main button
    float dropTop = pos.y + size.y + dropGap;
    
    for (size_t i = 0; i < options.size(); ++i) {
        float itemY = dropTop + padding + (itemHeight + itemGap) * i;
        
        // Items are plain rectangles (small corner radius, no outline) inside the container
        RoundedRectangleShape box({size.x - padding * 2.f, itemHeight}, Config::UI::Radius::Md);
        box.setPosition(sf::Vector2f(pos.x + padding, itemY));
        box.setFillColor(sf::Color::Transparent);
        box.setOutlineThickness(0.f);
        itemBoxes.push_back(box);

        sf::Text text(ctx.font, options[i], Config::UI::FONT_SIZE_BUTTON);
        text.setFillColor(textColor);
        
        sf::FloatRect bounds = text.getLocalBounds();
        text.setOrigin({
            bounds.position.x + bounds.size.x / 2.f, 
            bounds.position.y + bounds.size.y / 2.f
        });
        text.setPosition({pos.x + size.x / 2.f, itemY + itemHeight / 2.f});
        itemTexts.push_back(text);
        
        // Add divider line between items (not after the last one)
        if (i < options.size() - 1) {
            float divY = itemY + itemHeight + itemGap / 2.f;
            sf::RectangleShape div({size.x - padding * 2.f, 2.f});
            div.setPosition({pos.x + padding, divY});
            div.setFillColor(outlineColor);
            dividers.push_back(div);
        }
    }
    
    // Compute dropdown container background size
    float totalHeight = padding * 2.f + itemHeight * options.size() + itemGap * (options.size() > 0 ? options.size() - 1 : 0);
    dropdownBg.setSize({size.x, totalHeight});
    dropdownBg.setPosition({pos.x, dropTop});
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
        float dropGap = 4.f;
        float padding = 6.f;
        float itemGap = 4.f;
        float itemHeight = size.y - 4.f;
        float dropTop = pos.y + size.y + dropGap;
        
        size_t divIdx = 0;
        for (size_t i = 0; i < options.size(); ++i) {
            float itemY = dropTop + padding + (itemHeight + itemGap) * i;
            itemBoxes[i].setSize({size.x - padding * 2.f, itemHeight});
            itemBoxes[i].setPosition(sf::Vector2f(pos.x + padding, itemY));
            itemTexts[i].setPosition({pos.x + size.x / 2.f, itemY + itemHeight / 2.f});
            
            if (i < options.size() - 1 && divIdx < dividers.size()) {
                float divY = itemY + itemHeight + itemGap / 2.f;
                dividers[divIdx].setPosition({pos.x + padding, divY});
                dividers[divIdx].setSize({size.x - padding * 2.f, 2.f});
                ++divIdx;
            }
        }
        
        float totalHeight = padding * 2.f + itemHeight * options.size() + itemGap * (options.size() - 1);
        dropdownBg.setSize({size.x, totalHeight});
        dropdownBg.setPosition({pos.x, dropTop});
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
        arrowBox.setFillColor(hoverOutlineColor);
        mainBox.setOutlineColor(hoverOutlineColor);
    } else if (isHovered) {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));

        mainBox.setFillColor(hoverColor);
        arrowBox.setFillColor(hoverOutlineColor);
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
                itemBoxes[i].setFillColor(sf::Color::Transparent);
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
        ctx.window.draw(dropdownBg);
        for (const auto& box : itemBoxes) {
            ctx.window.draw(box);
        }
        for (const auto& div : dividers) {
            ctx.window.draw(div);
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
