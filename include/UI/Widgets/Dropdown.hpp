#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <Core/AppContext.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <vector>
#include <string>

namespace UI {
namespace Widgets {

class Dropdown {
public:
    Dropdown(AppContext& context, const std::string& label, sf::Vector2f position, sf::Vector2f size, const std::vector<std::string>& options);

    void handleEvent(const sf::Event::MouseButtonPressed& event);
    void update(sf::Vector2i mousePos);
    void draw() const;

    bool getIsDropped() const;
    int getSelectedIndex() const;
    std::string getSelectedText() const;

private:
    AppContext& ctx;
    bool isDropped;
    int selectedIndex;

    RoundedRectangleShape mainBox;
    RoundedRectangleShape arrowBox;
    sf::ConvexShape arrowTriangle;
    sf::Text mainText;

    std::vector<RoundedRectangleShape> itemBoxes;
    std::vector<sf::Text> itemTexts;
};

} // namespace Widgets
} // namespace UI
