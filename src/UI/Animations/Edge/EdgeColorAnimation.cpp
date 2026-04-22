#include "UI/Animations/Edge/EdgeColorAnimation.hpp"
#include <algorithm>

namespace UI::Animations {

    EdgeColorAnimation::EdgeColorAnimation(UI::DSA::Edge* edge, 
                                            sf::Color start, 
                                            sf::Color end, 
                                            float duration)
        : targetEdge(edge), 
          startColor(start), 
          endColor(end), 
          totalDuration(duration), 
          elapsedTime(0.0f) 
    {
        if (targetEdge) {
            targetEdge->setColor(startColor);
        }
    }

    sf::Color EdgeColorAnimation::lerpColor(sf::Color start, sf::Color end, float t) {
        return sf::Color(
            static_cast<std::uint8_t>(start.r + t * (end.r - start.r)),
            static_cast<std::uint8_t>(start.g + t * (end.g - start.g)),
            static_cast<std::uint8_t>(start.b + t * (end.b - start.b)),
            static_cast<std::uint8_t>(start.a + t * (end.a - start.a))
        );
    }

    void EdgeColorAnimation::update(float dt) {
        if (!targetEdge || isFinished()) return;

        elapsedTime += dt;
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        targetEdge->setColor(lerpColor(startColor, endColor, t));
    }

    bool EdgeColorAnimation::isFinished() const {
        return elapsedTime >= totalDuration;
    }

    void EdgeColorAnimation::reset() {
        elapsedTime = 0.f;
    }

}