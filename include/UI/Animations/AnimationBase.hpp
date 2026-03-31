#pragma once

namespace UI::Animations {

    class AnimationBase {
    public:
        virtual ~AnimationBase() = default;

        //Be called every frame in the main loop
        //dt: time between 2 frames
        virtual void update(float dt) = 0;

        //Return true if finished running
        virtual bool isFinished() const = 0;
    };

}