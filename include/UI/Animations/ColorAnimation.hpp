#pragma once
#include "UI/Animations/AnimationBase.hpp"
#include "UI/DSA/Node.hpp"
#include <SFML/Graphics/Color.hpp>

namespace UI::Animations {

    class ColorAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode;

        sf::Color startFill, endFill;
        sf::Color startText, endText;
        float totalDuration;
        float elapsedTime;

        sf::Color lerpColor(sf::Color start, sf::Color end, float t);

    public:
        ColorAnimation(UI::DSA::Node* node, 
                       sf::Color startFill, sf::Color endFill, 
                       sf::Color startText, sf::Color endText, 
                       float duration);
                       
        void update(float dt) override;
        bool isFinished() const override;
    };

}