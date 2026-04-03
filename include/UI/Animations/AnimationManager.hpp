#pragma once
#include "AnimationBase.hpp"
#include <vector>
#include <memory>

namespace UI::Animations {

    class AnimationManager {
    private:
        //list of running animations
        std::vector<std::unique_ptr<AnimationBase>> activeAnimations;

    public:

        bool empty() const;

        void addAnimation(std::unique_ptr<AnimationBase> anim);

        //update all animations and delete all the finished ones
        void update(float dt);

        //clear all (necessary when go to another screen)
        void clearAll(); 
    };

}