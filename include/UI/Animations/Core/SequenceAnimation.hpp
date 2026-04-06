#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <queue>
#include <memory>

namespace UI::Animations {

    class SequenceAnimation : public AnimationBase {
    private:
        std::queue<std::unique_ptr<AnimationBase>> sequence;

    public:
        SequenceAnimation() = default;

        void add(std::unique_ptr<AnimationBase> anim);
        void update(float dt) override;
        bool isFinished() const override;
    };

} // namespace UI::Animations