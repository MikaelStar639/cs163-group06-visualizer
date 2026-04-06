#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include "UI/DSA/Node.hpp"

namespace UI::Animations {

    class NodeMoveAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode;
        sf::Vector2f startPos;
        sf::Vector2f endPos;
        float totalDuration;
        float elapsedTime;

    public:
        NodeMoveAnimation(UI::DSA::Node* node, sf::Vector2f end, float duration);
        void update(float dt) override;
        bool isFinished() const override;
    };

} // namespace UI::Animations