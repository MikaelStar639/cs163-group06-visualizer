#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <queue>
#include <memory>

namespace UI::Animations {

    class SequenceAnimation : public AnimationBase {
    private:
        std::vector<std::unique_ptr<AnimationBase>> sequence;
        int currentIndex = 0;

    public:
        SequenceAnimation() = default;

        void add(std::unique_ptr<AnimationBase> anim);
        void update(float dt) override;
        bool isFinished() const override;
        void reset() override;
        bool isEmpty() const { return sequence.empty(); }
    };

} // namespace UI::Animations