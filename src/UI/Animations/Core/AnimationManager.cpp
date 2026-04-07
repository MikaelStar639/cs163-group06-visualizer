#include "UI/Animations/Core/AnimationManager.hpp"

namespace UI::Animations {

    bool AnimationManager::empty() const{
        return activeAnimations.empty();
    }
    
    void AnimationManager::addAnimation(std::unique_ptr<AnimationBase> anim) {
        if (anim) {
            activeAnimations.push_back(std::move(anim));
        }
    }

    void AnimationManager::update(float dt) {
        for (int i = static_cast<int>(activeAnimations.size()) - 1; i >= 0; --i) {
            //iterate backward because of the erase function
            activeAnimations[i]->update(dt);
            
            if (activeAnimations[i]->isFinished()) {
                activeAnimations.erase(activeAnimations.begin() + i);
            }
        }
    }

    void AnimationManager::clearAll() {
        activeAnimations.clear();
    }

}