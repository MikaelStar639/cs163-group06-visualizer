#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include "UI/DSA/Node.hpp"
#include <SFML/Graphics/Color.hpp>

namespace UI::Animations {

    class NodeColorAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode;

        sf::Color startFill, endFill;
        sf::Color startText, endText;
        float totalDuration;
        float elapsedTime;

        sf::Color lerpColor(sf::Color start, sf::Color end, float t);

    public:
        NodeColorAnimation(UI::DSA::Node* node, 
                       sf::Color startFill, sf::Color endFill, 
                       sf::Color startText, sf::Color endText, 
                       float duration);

        NodeColorAnimation(UI::DSA::Node* node, sf::Color targetFill, sf::Color targetText, float duration)
            : NodeColorAnimation(node, node->getFillColor(), targetFill, node->getLabelColor(), targetText, duration) {}
                       
        void update(float dt) override;
        bool isFinished() const override;
    };

    class NodeHighlightAnimation : public NodeColorAnimation {
    public:
        NodeHighlightAnimation(UI::DSA::Node* node, float duration = 0.3f)
            : NodeColorAnimation(
                node, 
                node->getFillColor(), Config::UI::Colors::NodeHighlight,
                node->getLabelColor(), Config::UI::Colors::LabelHighlight,
                duration
            ) {}
    };

    class NodeUnhighlightAnimation : public NodeColorAnimation {
    public:
        NodeUnhighlightAnimation(UI::DSA::Node* node, float duration = 0.3f)
            : NodeColorAnimation(
                node, 
                node->getFillColor(), Config::UI::Colors::NodeFill,
                node->getLabelColor(), Config::UI::Colors::NodeText,
                duration
            ) {}
    };
}