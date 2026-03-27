#include "UI/Widgets/Dropdown.hpp"

namespace UI {
namespace Widgets {

Dropdown::Dropdown(sf::Vector2f position, sf::Vector2f size, const sf::Font& font, const std::vector<std::string>& options) 
    : isDropped(false), selectedIndex(-1) 
{
    // Setup Main Box
    mainBox.setPosition(position);
    mainBox.setSize(size);
    mainBox.setFillColor(sf::Color(50, 50, 50));
    mainBox.setOutlineColor(sf::Color::White);
    mainBox.setOutlineThickness(1.f);

    // Setup Main Text
    mainText.setFont(font);
    mainText.setCharacterSize(static_cast<unsigned int>(size.y * 0.5f));
    mainText.setFillColor(sf::Color::White);
    if (!options.empty()) {
        mainText.setString("Select an option...");
    }
    
    // SFML 3.x text bounds positioning
    sf::FloatRect textBounds = mainText.getLocalBounds();
    mainText.setPosition(sf::Vector2f(
        position.x + 10.f, 
        position.y + (size.y - textBounds.size.y) / 2.f - textBounds.position.y
    ));

    // Setup Options
    for (size_t i = 0; i < options.size(); ++i) {
        sf::RectangleShape box;
        box.setPosition(sf::Vector2f(position.x, position.y + size.y * (i + 1)));
        box.setSize(size);
        box.setFillColor(sf::Color(70, 70, 70));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(1.f);
        itemBoxes.push_back(box);

        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(mainText.getCharacterSize());
        text.setFillColor(sf::Color::White);
        text.setString(options[i]);
        
        sf::FloatRect optionBounds = text.getLocalBounds();
        text.setPosition(sf::Vector2f(
            box.getPosition().x + 10.f, 
            box.getPosition().y + (size.y - optionBounds.size.y) / 2.f - optionBounds.position.y
        ));
        itemTexts.push_back(text);
    }
}

void Dropdown::handleEvent(const sf::Event::MouseButtonPressed& event, const sf::RenderWindow& window) {
    if (event.button == sf::Mouse::Button::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        bool clickedInsideMainBox = mainBox.getGlobalBounds().contains(mousePos);

        // 1. Toggle when clicking main box
        if (clickedInsideMainBox) {
            isDropped = !isDropped;
            return;
        }

        // 2. Click outside or select item
        if (isDropped) {
            bool clickedAnyItem = false;

            for (size_t i = 0; i < itemBoxes.size(); ++i) {
                if (itemBoxes[i].getGlobalBounds().contains(mousePos)) {
                    selectedIndex = static_cast<int>(i);
                    mainText.setString(itemTexts[i].getString());
                    isDropped = false;
                    clickedAnyItem = true;
                    break;
                }
            }

            if (!clickedAnyItem) {
                isDropped = false; // Click outside
            }
        }
    }
}

void Dropdown::draw(sf::RenderWindow& window) const {
    window.draw(mainBox);
    window.draw(mainText);

    if (isDropped) {
        for (const auto& box : itemBoxes) {
            window.draw(box);
        }
        for (const auto& text : itemTexts) {
            window.draw(text);
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
    if (selectedIndex >= 0 && selectedIndex < itemTexts.size()) {
        return itemTexts[selectedIndex].getString();
    }
    return "";
}

} // namespace Widgets
} // namespace UI
