#pragma once
#include "UI/Animations/ScaleAnimation.hpp"
#include <functional>

namespace UI::Animations {

    class DeleteAnimation : public ScaleAnimation {
    private:
        std::function<void()> onComplete;

    public:
        //callback (ex: pop_back(), erase(),...) used to delete the Node after animation
        DeleteAnimation(UI::DSA::Node* node, float duration, std::function<void()> callback)
            : ScaleAnimation(node, node ? node->getScale() : 1.0f, 0.0f, duration),
              onComplete(callback) {}

        ~DeleteAnimation() override {
            if (onComplete) onComplete();
        }
    };

}