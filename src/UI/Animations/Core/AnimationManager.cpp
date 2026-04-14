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
        if (paused) return;

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

    void AnimationManager::togglePause() {
        paused = !paused; 
    }

    bool AnimationManager::isPaused() const {
        return paused; 
    }

    void AnimationManager::setPaused(bool p) {
        paused = p; 
    }

    void AnimationManager::skipToEnd() {
        if (!activeAnimations.empty()) {
            bool wasPaused = paused;
            paused = false;     
            update(9999.0f);    
            paused = wasPaused; 
        }
    }

}