#include "UI/Shapes/StopIcon.hpp"

namespace UI::Shapes {
    StopIcon::StopIcon(float size) {
        square.setSize({size, size});
        square.setOrigin({size / 2.f, size / 2.f});
    }

    void StopIcon::setFillColor(const sf::Color& color) {
        square.setFillColor(color);
    }

    void StopIcon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        target.draw(square, states);
    }
}
