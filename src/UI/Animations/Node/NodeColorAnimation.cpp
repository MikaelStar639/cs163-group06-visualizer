#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include <algorithm>
#include <cstdint>

namespace UI::Animations {

    NodeColorAnimation::NodeColorAnimation(UI::DSA::Node* node, 
                                   sf::Color startF, sf::Color endF,
                                   sf::Color startT, sf::Color endT, 
                                   float duration)
            : targetNode(node), 
              startFill(startF), endFill(endF), 
              startText(startT), endText(endT),
              totalDuration(duration), elapsedTime(0.f) 
    {}

    sf::Color NodeColorAnimation::lerpColor(sf::Color start, sf::Color end, float t) {
        std::uint8_t r = static_cast<std::uint8_t>(start.r + t * (end.r - start.r));
        std::uint8_t g = static_cast<std::uint8_t>(start.g + t * (end.g - start.g));
        std::uint8_t b = static_cast<std::uint8_t>(start.b + t * (end.b - start.b));
        std::uint8_t a = static_cast<std::uint8_t>(start.a + t * (end.a - start.a));
        return sf::Color(r, g, b, a);
    }

    void NodeColorAnimation::update(float dt) {
        if (!targetNode || isFinished()) return;

        elapsedTime += dt;
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        targetNode->setFillColor(lerpColor(startFill, endFill, t));
        targetNode->setLabelColor(lerpColor(startText, endText, t));
    }

    bool NodeColorAnimation::isFinished() const {
        return elapsedTime >= totalDuration;
    }

    void NodeColorAnimation::reset() {
        elapsedTime = 0.f;
    }

}