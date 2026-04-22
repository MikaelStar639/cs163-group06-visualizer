#include "UI/Animations/Core/ParallelAnimation.hpp"

namespace UI::Animations {
    void ParallelAnimation::add(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            animations.push_back(std::move(anim));
        }
    }

    void ParallelAnimation::update(float dt){
        for (auto& anim : animations) {
            if (!anim->isFinished()) {
                anim->update(dt);
            }
        }
    }

    bool ParallelAnimation::isFinished() const{
        for (const auto& anim : animations) {
            if (!anim->isFinished()) return false;
        }
        return true;
    }

    void ParallelAnimation::reset() {
        for (auto& anim : animations) {
            anim->reset();
        }
    }

} // namespace UI::Animations