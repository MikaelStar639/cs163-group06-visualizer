#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include <vector>
#include <memory>

namespace UI::Animations {

    class AnimationManager {
    private:
        std::vector<std::unique_ptr<AnimationBase>> activeAnimations;
        float speedScale = 1.0f;
        bool paused = false;

    public:

        bool empty() const;

        void addAnimation(std::unique_ptr<AnimationBase> anim);

        //update all animations and delete all the finished ones
        void update(float dt);

        //clear all (necessary when go to another screen)
        void clearAll(); 

        void setSpeedScale(float scale);
        float getSpeedScale() const;

        void togglePause();
        bool isPaused() const;
        void setPaused(bool p);
        
        void skipToEnd();
    };

}