#include "UI/Animations/Core/SequenceAnimation.hpp"

namespace UI::Animations {

    void SequenceAnimation::add(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            sequence.push_back(std::move(anim));
        }
    }

    void SequenceAnimation::update(float dt) {
        while (currentIndex < static_cast<int>(sequence.size()) && dt > 0.f) {
            sequence[currentIndex]->update(dt);
            if (sequence[currentIndex]->isFinished()) {
                currentIndex++;
            } 
            else {
                break;
            }
        }
    }

    bool SequenceAnimation::isFinished() const {
        return currentIndex >= static_cast<int>(sequence.size());
    }

    void SequenceAnimation::reset() {
        currentIndex = 0;
        for (auto& anim : sequence) {
            anim->reset();
        }
    }

} // namespace UI::Animations