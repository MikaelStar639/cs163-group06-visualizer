#include "UI/Animations/Core/ParallelAnimation.hpp"

namespace UI::Animations {
    void ParallelAnimation::add(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            animations.push_back(std::move(anim));
        }
    }

    void ParallelAnimation::update(float dt){
        for (int i = static_cast<int>(animations.size()) - 1; i >= 0; --i) {
            animations[i]->update(dt);
            
            if (animations[i]->isFinished()) {
                animations.erase(animations.begin() + i);
            }
        }
    }

    bool ParallelAnimation::isFinished() const{
        return animations.empty();
    }

} // namespace UI::Animations