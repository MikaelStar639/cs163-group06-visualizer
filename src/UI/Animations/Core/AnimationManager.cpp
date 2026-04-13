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
        float scaledDt = dt * speedScale;
        for (int i = static_cast<int>(activeAnimations.size()) - 1; i >= 0; --i) {
            activeAnimations[i]->update(scaledDt);
            
            if (activeAnimations[i]->isFinished()) {
                activeAnimations.erase(activeAnimations.begin() + i);
            }
        }
    }

    void AnimationManager::clearAll() {
        activeAnimations.clear();
    }

    void AnimationManager::setSpeedScale(float scale) {
        speedScale = scale; 
    }
    
    float AnimationManager::getSpeedScale() const {
        return speedScale; 
    }

}