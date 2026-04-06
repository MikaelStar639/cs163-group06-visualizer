#include "UI/Animations/Node/NodeSwapAnimation.hpp"
#include <algorithm>
#include <cmath> 

namespace UI::Animations{

    NodeSwapAnimation::NodeSwapAnimation(UI::DSA::Node* a, UI::DSA::Node* b, float duration)
            : nodeA(a), nodeB(b), totalDuration(duration), elapsedTime(0.f) 
    {
        if (nodeA && nodeB) {
            startPosA = nodeA->getPosition();
            startPosB = nodeB->getPosition();
        }
    }

    void NodeSwapAnimation::update(float dt) {
            if (!nodeA || !nodeB || isFinished()) return;

            elapsedTime += dt;
            float t = std::min(elapsedTime / totalDuration, 1.0f);

            // Base Lerp
            sf::Vector2f lerpA = startPosA + t * (startPosB - startPosA);
            sf::Vector2f lerpB = startPosB + t * (startPosA - startPosB);

            
            sf::Vector2f diff = startPosB - startPosA;
            float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            
            if (distance > 0.001f) {
                // normal vector
                sf::Vector2f normal(-diff.y / distance, diff.x / distance);

                // some math 🥀
                float arcHeight = std::sin(t * PI) * (distance / 2.0f);

                sf::Vector2f currentPosA = lerpA + normal * arcHeight;
                sf::Vector2f currentPosB = lerpB - normal * arcHeight;

                nodeA->setPosition(currentPosA);
                nodeB->setPosition(currentPosB);
            }
        }

    bool NodeSwapAnimation::isFinished() const {
            return elapsedTime >= totalDuration;
        }
}