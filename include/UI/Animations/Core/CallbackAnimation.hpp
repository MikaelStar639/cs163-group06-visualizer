#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <functional>

namespace UI::Animations {

    class CallbackAnimation : public AnimationBase {
    private:
        std::function<void()> callback;
        bool hasRun = false;

    public:
        explicit CallbackAnimation(std::function<void()> cb);
        void update(float dt) override;
        bool isFinished() const override;
        void reset() override;
    };

} // namespace UI::Animations