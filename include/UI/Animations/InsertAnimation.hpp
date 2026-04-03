#pragma once
#include "UI/Animations/ScaleAnimation.hpp"

namespace UI::Animations {

    class InsertAnimation : public ScaleAnimation {
    public:
        InsertAnimation(UI::DSA::Node* node, float duration)
            : ScaleAnimation(node, 0.0f, 1.0f, duration) {}
    };

}