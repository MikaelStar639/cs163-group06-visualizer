#pragma once
#include "UI/Animations/ColorAnimation.hpp"
#include "Core/Constants.hpp"

namespace UI::Animations {

    class UnhighlightAnimation : public ColorAnimation {
    public:
        UnhighlightAnimation(UI::DSA::Node* node, float duration = 0.3f)
            : ColorAnimation(
                node, 
                node->getFillColor(), Config::UI::Colors::NodeFill,
                node->getLabelColor(), Config::UI::Colors::NodeText,
                duration
            ) {}
    };

}