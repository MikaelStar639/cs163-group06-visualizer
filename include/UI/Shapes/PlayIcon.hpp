#pragma once
#include <SFML/Graphics.hpp>

namespace UI::Shapes {

    class PlayIcon : public sf::Drawable, public sf::Transformable {
    private:
        sf::ConvexShape triangle;

    public:
        PlayIcon(float radius = 8.f);

        void setFillColor(const sf::Color& color);

    protected:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
    };

} // namespace UI::Shapes