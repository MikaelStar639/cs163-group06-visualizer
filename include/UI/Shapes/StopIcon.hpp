#pragma once
#include <SFML/Graphics.hpp>

namespace UI::Shapes {
    class StopIcon : public sf::Drawable, public sf::Transformable {
    private:
        sf::RectangleShape square;
    public:
        StopIcon(float size = 16.f);
        void setFillColor(const sf::Color& color);
    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };
}
