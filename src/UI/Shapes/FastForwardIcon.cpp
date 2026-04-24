#include "UI/Shapes/FastForwardIcon.hpp"

namespace UI::Shapes {
    FastForwardIcon::FastForwardIcon(float radius) {
        float triWidth = radius * 1.5f;
        float triangleGap = 10.f; // Balanced gap for visual clarity
        float totalWidth = triWidth + triangleGap;

        auto setupTriangle = [&](sf::ConvexShape& tri, float offsetX) {
            tri.setPointCount(3);
            tri.setPoint(0, {0.f, -radius});
            tri.setPoint(1, {triWidth, 0.f});
            tri.setPoint(2, {0.f, radius});
            tri.setOrigin({0.f, 0.f});
            tri.setPosition({offsetX, 0.f});
            tri.setFillColor(sf::Color::White);
        };

        // Triangle 1 (Left)
        setupTriangle(triangle1, -totalWidth / 2.f);
        
        // Triangle 2 (Right)
        setupTriangle(triangle2, -totalWidth / 2.f + triangleGap);
    }

    void FastForwardIcon::setFillColor(const sf::Color& color) {
        triangle1.setFillColor(color);
        triangle2.setFillColor(color);
    }

    void FastForwardIcon::draw(sf::RenderTarget& target, sf::RenderStates states) const {
        states.transform *= getTransform();
        target.draw(triangle1, states);
        target.draw(triangle2, states);
    }
}
