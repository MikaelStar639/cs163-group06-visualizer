#include "UI/Animations/MoveAnimation.hpp"
#include <algorithm>

namespace UI::Animations{
    MoveAnimation::MoveAnimation(UI::DSA::Node* node, sf::Vector2f end, float duration)
        : targetNode(node), endPos(end), totalDuration(duration), elapsedTime(0.f) 
    {
        if (targetNode) {
            startPos = targetNode->getPosition(); 
        }
    }

    void MoveAnimation::update(float dt) {
        if (!targetNode || isFinished()) return;

        elapsedTime += dt;
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        sf::Vector2f currentPos = startPos + t * (endPos - startPos);
        targetNode->setPosition(currentPos);
    }

    bool MoveAnimation::isFinished() const {
        return elapsedTime >= totalDuration;
    }
}