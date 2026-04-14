#include "UI/Shapes/PauseIcon.hpp"

namespace UI::Shapes {

    PauseIcon::PauseIcon(float width, float height, float gap) {
        bar1.setSize({width, height});
        bar1.setOrigin({width / 2.f, height / 2.f});
        bar1.setPosition({-gap, 0.f}); 
        bar1.setFillColor(sf::Color::White);

        bar2.setSize({width, height});
        bar2.setOrigin({width / 2.f, height / 2.f});
        bar2.setPosition({gap, 0.f});  
        bar2.setFillColor(sf::Color::White);
    }

    void PauseIcon::setFillColor(const sf::Color& color) {
        bar1.setFillColor(color);
        bar2.setFillColor(color);
    }

    void PauseIcon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        target.draw(bar1, states);
        target.draw(bar2, states);
    }

} // namespace UI::Shapes