#include "UI/Animations/Core/CallbackAnimation.hpp"

namespace UI::Animations {

    CallbackAnimation::CallbackAnimation(std::function<void()> cb) : callback(cb) {}

    void CallbackAnimation::update(float dt) {
        if (!hasRun) {
            if (callback) callback(); 
            hasRun = true;
        }
    }

    bool CallbackAnimation::isFinished() const { 
        return hasRun; 
    }

    void CallbackAnimation::reset() {
        hasRun = false;
    }

} // namespace UI::Animations