#pragma once
#include <SFML/Graphics.hpp>

namespace UI::Shapes {
    /**
     * @brief A Fast Forward icon consisting of two overlapping triangles.
     */
    class FastForwardIcon : public sf::Drawable, public sf::Transformable {
    private:
        sf::ConvexShape triangle1;
        sf::ConvexShape triangle2;

    public:
        FastForwardIcon(float radius = 8.f);
        void setFillColor(const sf::Color& color);

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}
