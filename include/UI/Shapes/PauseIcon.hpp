#pragma once
#include <SFML/Graphics.hpp>

namespace UI::Shapes {

    class PauseIcon : public sf::Drawable, public sf::Transformable {
    private:
        sf::RectangleShape bar1;
        sf::RectangleShape bar2;

    public:
        PauseIcon(float width = 5.f, float height = 16.f, float gap = 4.f);

        void setFillColor(const sf::Color& color);

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

} // namespace UI::Shapes