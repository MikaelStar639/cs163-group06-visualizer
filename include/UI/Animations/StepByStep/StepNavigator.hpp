#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include "UI/Animations/Core/AnimationManager.hpp"
#include <vector>
#include <memory>
#include <functional>

namespace UI::Animations {

    /**
     * @brief Manages navigation through discrete logical animation steps.
     * 
     * This class allows algorithms to be broken down into segments that can be
     * triggered one by one (Forward navigation).
     */
    class StepNavigator {
    private:
        std::vector<std::unique_ptr<AnimationBase>> steps;
        int currentStepIndex = -1;
        bool stepModeActive = false;
        
        AnimationManager* animManager = nullptr;

    public:
        StepNavigator() = default;

        void setAnimationManager(AnimationManager* manager) { animManager = manager; }

        /**
         * @brief Clear all steps and reset index.
         */
        void clear();

        /**
         * @brief Add a logical animation step to the queue.
         */
        void addStep(std::unique_ptr<AnimationBase> step);

        /**
         * @brief Trigger the next logical step.
         * @return true if a step was started, false if no more steps remain.
         */
        bool playNext();

        /**
         * @brief Check if there are more steps to play.
         */
        bool hasNext() const;

        /**
         * @brief Enable or disable step-by-step mode.
         */
        void setStepMode(bool active) { stepModeActive = active; }
        bool isStepMode() const { return stepModeActive; }

        int getCurrentIndex() const { return currentStepIndex; }
        int getTotalSteps() const { return static_cast<int>(steps.size()); }
        
        /**
         * @brief Autonomous update to handle auto-advancement when step-by-step is OFF.
         */
        void update(float dt);

        /**
         * @brief Plays all remaining steps sequentially through the animation manager.
         */
        void playAllRemaining();

        /**
         * @brief Skips all remaining steps and plays them instantly.
         */
        void skipAll();
    };

} // namespace UI::Animations
