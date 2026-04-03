#pragma once
#include "UI/Animations/AnimationBase.hpp"
#include "UI/DSA/Node.hpp"

namespace UI::Animations {

    //Move the Nodes in circle shape
    class SwapAnimation : public AnimationBase {
    private:
        UI::DSA::Node* nodeA;
        UI::DSA::Node* nodeB;
        
        sf::Vector2f startPosA;
        sf::Vector2f startPosB;
        
        float totalDuration;
        float elapsedTime;

        const float PI = 3.14159265f; 

    public:
        SwapAnimation(UI::DSA::Node* a, UI::DSA::Node* b, float duration);
        void update(float dt) override;
        bool isFinished() const override;
    };

} // namespace UI::Animations