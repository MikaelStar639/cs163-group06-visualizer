#include "UI/Animations/ColorAnimation.hpp"
#include <algorithm>
#include <cstdint> 

namespace UI::Animations {

    ColorAnimation::ColorAnimation(UI::DSA::Node* node, sf::Color start, sf::Color end, float duration)
            : targetNode(node), startColor(start), endColor(end), 
              totalDuration(duration), elapsedTime(0.f) 
    {
        if (targetNode) {
            targetNode->setFillColor(startColor); 
        }
    }

    void ColorAnimation::update(float dt) {
        if (!targetNode || isFinished()) return;

        elapsedTime += dt;
        
        // Tính % thời gian chạy từ 0.0 đến 1.0
        float t = std::min(elapsedTime / totalDuration, 1.0f);

        std::uint8_t r = static_cast<std::uint8_t>(startColor.r + t * (endColor.r - startColor.r));
        std::uint8_t g = static_cast<std::uint8_t>(startColor.g + t * (endColor.g - startColor.g));
        std::uint8_t b = static_cast<std::uint8_t>(startColor.b + t * (endColor.b - startColor.b));
        std::uint8_t a = static_cast<std::uint8_t>(startColor.a + t * (endColor.a - startColor.a));

        targetNode->setFillColor(sf::Color(r, g, b, a));
    }

    bool ColorAnimation::isFinished() const {
        return elapsedTime >= totalDuration;
    }

} // namespace UI::Animations