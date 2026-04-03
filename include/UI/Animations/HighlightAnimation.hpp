#pragma once
#include "UI/Animations/ColorAnimation.hpp"
#include "Core/Constants.hpp"

namespace UI::Animations {

    class HighlightAnimation : public ColorAnimation {
    public:
        HighlightAnimation(UI::DSA::Node* node, float duration = 0.3f)
            : ColorAnimation(
                node, 
                node->getFillColor(), Config::UI::Colors::NodeHighlight,
                node->getLabelColor(), Config::UI::Colors::LabelHighlight,
                duration
            ) {}
    };

}