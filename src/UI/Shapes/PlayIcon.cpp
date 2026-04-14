#include "UI/Shapes/PlayIcon.hpp"

namespace UI::Shapes {

    PlayIcon::PlayIcon(float radius) {
        triangle.setPointCount(3);
        triangle.setPoint(0, {0.f, -radius});
        triangle.setPoint(1, {radius * 1.5f, 0.f});
        triangle.setPoint(2, {0.f, radius});

        triangle.setOrigin({radius * 0.5f, 0.f}); 
        triangle.setFillColor(sf::Color::White);
    }

    void PlayIcon::setFillColor(const sf::Color& color) {
        triangle.setFillColor(color);
    }

    void PlayIcon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        target.draw(triangle, states);
    }

} // namespace UI::Shapes