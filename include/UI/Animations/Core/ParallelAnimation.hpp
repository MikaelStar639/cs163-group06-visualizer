#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <vector>
#include <memory>

namespace UI::Animations {

    class ParallelAnimation : public AnimationBase {
    private:
        std::vector<std::unique_ptr<AnimationBase>> animations;

    public:
        ParallelAnimation() = default;
        void add(std::unique_ptr<AnimationBase> anim);
        void update(float dt) override;
        bool isFinished() const override;
        void reset() override;
    };

} // namespace UI::Animations