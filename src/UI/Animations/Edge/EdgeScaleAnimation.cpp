#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include <algorithm>

namespace UI::Animations {

    EdgeScaleAnimation::EdgeScaleAnimation(UI::DSA::Edge* edge, float start, float end, float duration)
        : targetEdge(edge), 
          startThickness(start), 
          endThickness(end), 
          totalDuration(duration), 
          elapsedTime(0.0f) 
    {
        if (targetEdge) {
            targetEdge->setThickness(startThickness);
        }
    }

    void EdgeScaleAnimation::update(float dt) {
        if (!targetEdge || isFinished()) return;

        elapsedTime += dt;

        // % time [0.0 - 1.0]
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        float currentThickness = startThickness + t * (endThickness - startThickness);
        
        targetEdge->setThickness(currentThickness);
    }

    bool EdgeScaleAnimation::isFinished() const {
        return elapsedTime >= totalDuration;
    }

}