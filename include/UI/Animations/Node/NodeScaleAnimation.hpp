#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include "UI/DSA/Node.hpp" 

namespace UI::Animations {

    class NodeScaleAnimation : public AnimationBase {
    private:
        UI::DSA::Node* targetNode; 

        float startScale;       
        float endScale;         
        float totalDuration;    
        float elapsedTime;      

    public:
        NodeScaleAnimation(UI::DSA::Node* node, float start, float end, float duration);
        void update(float dt) override;
        bool isFinished() const override;
        void reset() override;
    };

    class NodeInsertAnimation : public NodeScaleAnimation {
    public:
        NodeInsertAnimation(UI::DSA::Node* node, float duration)
            : NodeScaleAnimation(node, 0.0f, 1.0f, duration) {}
    };

    class NodeDeleteAnimation : public NodeScaleAnimation {
    private:
        std::function<void()> onComplete;

    public:
        //callback (ex: pop_back(), erase(),...) used to delete the Node after animation
        NodeDeleteAnimation(UI::DSA::Node* node, float duration, std::function<void()> callback)
            : NodeScaleAnimation(node, node ? node->getScale() : 1.0f, 0.0f, duration),
              onComplete(callback) {}

        ~NodeDeleteAnimation() override {
            if (onComplete) onComplete();
        }
    };

} // namespace UI::Animations