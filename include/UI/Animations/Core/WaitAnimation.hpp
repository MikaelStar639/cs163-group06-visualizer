#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"

namespace UI::Animations {

    // Simple animation that does nothing but wait for a duration.
    // Useful for pacing pseudo-code line highlights.
    class WaitAnimation : public AnimationBase {
    private:
        float duration;
        float elapsed = 0.f;

    public:
        explicit WaitAnimation(float duration) : duration(duration) {}
        void update(float dt) override { elapsed += dt; }
        bool isFinished() const override { return elapsed >= duration; }
    };

} // namespace UI::Animations
