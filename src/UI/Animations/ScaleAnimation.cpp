#include "UI/Animations/ScaleAnimation.hpp"
#include <algorithm> 

namespace UI::Animations {

    ScaleAnimation::ScaleAnimation(UI::DSA::Node* node, float start, float end, float duration)
            : targetNode(node), startScale(start), endScale(end), 
              totalDuration(duration), elapsedTime(0.f) 
    {
        if (targetNode) {
            targetNode->setScale(startScale); 
        }
    }

    void ScaleAnimation::update(float dt) {
        if (!targetNode || isFinished()) return;

        elapsedTime += dt;
        
        // % time [0.0 - 1.0]
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        float currentScale = startScale + t * (endScale - startScale);

        targetNode->setScale(currentScale);
    }

    bool ScaleAnimation::isFinished() const{
        return elapsedTime >= totalDuration;
    }
}