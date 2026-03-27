#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <vector>
#include <string>

namespace UI {
namespace Widgets {

class Dropdown {
public:
    Dropdown(sf::Vector2f position, sf::Vector2f size, const sf::Font& font, const std::vector<std::string>& options);

    void handleEvent(const sf::Event::MouseButtonPressed& event, const sf::RenderWindow& window);
    void draw(sf::RenderWindow& window) const;

    bool getIsDropped() const;
    int getSelectedIndex() const;
    std::string getSelectedText() const;

private:
    bool isDropped;
    int selectedIndex;

    sf::RectangleShape mainBox;
    sf::Text mainText;

    std::vector<sf::RectangleShape> itemBoxes;
    std::vector<sf::Text> itemTexts;
};

} // namespace Widgets
} // namespace UI
