#pragma once
#include "UI/Animations/Core/AnimationBase.hpp"
#include "UI/Animations/Core/AnimationManager.hpp"
#include <any>
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
        std::vector<std::shared_ptr<AnimationBase>> steps;
        int currentStepIndex = -1;
        bool stepModeActive = true;
        
        AnimationManager* animManager = nullptr;
        bool autoPlayEnabled = false;
        float autoPlayDelay = 1.2f;
        float autoPlayTimer = 0.0f;

        // History of states for the Back feature (Generic via std::any)
        std::vector<std::any> history;
        std::function<std::any()> snapshotProvider;
        std::function<void(const std::any&)> snapshotRestorer;

    public:
        StepNavigator() = default;

        void setAnimationManager(AnimationManager* manager) { animManager = manager; }

        /**
         * @brief Register callbacks to save/restore state.
         */
        void setSnapshotHandlers(std::function<std::any()> saver, 
                                 std::function<void(const std::any&)> restorer) {
            snapshotProvider = saver;
            snapshotRestorer = restorer;
        }

        /**
         * @brief Clear all steps and reset index.
         */
        void clear();

        /**
         * @brief Add a logical animation step to the queue.
         */
        void addStep(std::shared_ptr<AnimationBase> step);

        /**
         * @brief Trigger the next logical step.
         * @return true if a step was started, false if no more steps remain.
         */
        bool playNext(bool skipCurrent = true, bool disableSnapshot = false);

        /**
         * @brief Restore the previous state.
         * @return true if stepped back successfully.
         */
        bool stepBack();

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
        
        void setAutoPlay(bool enabled) { autoPlayEnabled = enabled; if (enabled) autoPlayTimer = 0.0f; }
        bool isAutoPlay() const { return autoPlayEnabled; }
        void setAutoPlayDelay(float delay) { autoPlayDelay = delay; }

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

        /**
         * @brief Instantly finish all remaining steps without "smooth landing".
         */
        void forceFinishAll();

        /**
         * @brief Restores the state to the very beginning (history[0]) and clears navigation.
         * Useful for the Cancel operation.
         */
        void restoreToStart();
    };

} // namespace UI::Animations
