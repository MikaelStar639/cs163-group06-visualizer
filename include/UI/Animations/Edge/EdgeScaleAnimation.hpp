#pragma once
#include "../Core/AnimationBase.hpp"
#include "UI/DSA/Edge.hpp"
#include <SFML/Graphics.hpp>
#include <functional>

namespace UI::Animations {

    class EdgeScaleAnimation : public AnimationBase {
    public:
        EdgeScaleAnimation(UI::DSA::Edge* edge, float start, float end, float duration);

        void update(float dt) override;
        bool isFinished() const override;

    private:
        UI::DSA::Edge* targetEdge;
        float startThickness;
        float endThickness;
        float totalDuration;
        float elapsedTime;
    };

    class EdgeInsertAnimation : public EdgeScaleAnimation {
    public:
        // Automatically starts at 0.0f thickness and grows to 5.0f (or your preferred max)
        EdgeInsertAnimation(UI::DSA::Edge* edge, float duration)
            : EdgeScaleAnimation(edge, 0.0f, 5.0f, duration) {}
    };

    class EdgeDeleteAnimation : public EdgeScaleAnimation {
    private:
        std::function<void()> onComplete;

    public:
        // callback (ex: edges.erase(), edges.pop_back()) used to delete the Edge after animation
        EdgeDeleteAnimation(UI::DSA::Edge* edge, float duration, std::function<void()> callback)
            : EdgeScaleAnimation(edge, edge ? edge->getThickness() : 5.0f, 0.0f, duration),
              onComplete(callback) {}

        ~EdgeDeleteAnimation() override {
            if (onComplete) onComplete();
        }
    };
}