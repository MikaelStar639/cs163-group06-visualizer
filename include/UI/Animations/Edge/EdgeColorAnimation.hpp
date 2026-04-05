#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <SFML/Graphics.hpp>
#include "UI/DSA/Edge.hpp"

namespace UI::Animations {

    class EdgeColorAnimation : public AnimationBase  {
    public:
        EdgeColorAnimation(UI::DSA::Edge* edge, 
                                                    sf::Color start, 
                                                    sf::Color end, 
                                                    float duration);

        void update(float dt);
        bool isFinished() const;

    private:
        sf::Color lerpColor(sf::Color start, sf::Color end, float t);

        UI::DSA::Edge* targetEdge;
        sf::Color startColor;
        sf::Color endColor;
        
        float totalDuration;
        float elapsedTime;
    };

    class EdgeHighlightAnimation : public EdgeColorAnimation {
    public:
        EdgeHighlightAnimation(UI::DSA::Edge* edge, float duration = 0.3f)
            : EdgeColorAnimation(
                edge,
                edge ? edge->getColor() : sf::Color::White, // Start: current color
                Config::UI::Colors::EdgeHighlight,          // End: highlight constant
                duration
            ) {}
    };

    class EdgeUnhighlightAnimation : public EdgeColorAnimation {
    public:
        EdgeUnhighlightAnimation(UI::DSA::Edge* edge, float duration = 0.3f)
            : EdgeColorAnimation(
                edge,
                edge ? edge->getColor() : sf::Color::White, // Start: Current color
                Config::UI::Colors::EdgeFill,           // End: Default Grey
                duration
            ) {}
    };
}