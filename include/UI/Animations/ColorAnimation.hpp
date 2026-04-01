#pragma once
#include "UI/Animations/AnimationBase.hpp"
#include "UI/DSA/Node.hpp"
#include <SFML/Graphics/Color.hpp>

namespace UI::Animations {

    class ColorAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode;

        sf::Color startColor;
        sf::Color endColor;
        float totalDuration;
        float elapsedTime;

    public:
        ColorAnimation(UI::DSA::Node* node, sf::Color start, sf::Color end, float duration);
        void update(float dt) override;
        bool isFinished() const override;
    };

} // namespace UI::Animations