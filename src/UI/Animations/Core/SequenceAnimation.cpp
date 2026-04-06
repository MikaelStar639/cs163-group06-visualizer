#include "UI/Animations/Core/SequenceAnimation.hpp"

namespace UI::Animations {

    void SequenceAnimation::add(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            sequence.push(std::move(anim));
        }
    }

    void SequenceAnimation::update(float dt) {
        if (sequence.empty()) return;

        sequence.front()->update(dt);

        if (sequence.front()->isFinished()) {
            sequence.pop();
        }
    }

    bool SequenceAnimation::isFinished() const {
        return sequence.empty();
    }

} // namespace UI::Animations