#include "UI/Animations/StepByStep/StepNavigator.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"

namespace UI::Animations {

    // Internal proxy to allow sharing AnimationBase ownership with AnimationManager
    class SharedAnimationProxy : public AnimationBase {
    private:
        std::shared_ptr<AnimationBase> inner;
    public:
        SharedAnimationProxy(std::shared_ptr<AnimationBase> anim) : inner(anim) {
            if (inner) inner->reset();
        }
        void update(float dt) override { if (inner) inner->update(dt); }
        bool isFinished() const override { return !inner || inner->isFinished(); }
        void reset() override { if (inner) inner->reset(); }
    };

    void StepNavigator::clear() {
        steps.clear();
        history.clear();
        currentStepIndex = -1;
        autoPlayEnabled = false;
        autoPlayTimer = 0.0f;
    }

    void StepNavigator::addStep(std::shared_ptr<AnimationBase> step) {
        if (step) {
            steps.push_back(step); // Keep shared ownership
        }
    }

    void StepNavigator::update(float dt) {
        if (!animManager) return;

        // Mode 1: Step-by-Step OFF (Traditional continuous playback)
        if (!stepModeActive && hasNext() && animManager->empty() && !animManager->isPaused()) {
            autoPlayTimer += dt;
            if (autoPlayTimer >= 0.25f) { // Subtle but important delay
                autoPlayTimer = 0.0f;
                playNext();
            }
        }
        
        // Mode 2: Auto-Play in Step Mode (The "Movie" mode)
        else if (stepModeActive && autoPlayEnabled && hasNext() && animManager->empty()) {
            autoPlayTimer += dt;
            if (autoPlayTimer >= autoPlayDelay) {
                autoPlayTimer = 0.0f;
                
                // Preserve the enabled state since playNext() would normally disable it
                bool wasAuto = autoPlayEnabled;
                playNext();
                autoPlayEnabled = wasAuto;
            }
        }
        else {
            autoPlayTimer = 0.0f; // Reset if interrupted or animating
        }
    }

    bool StepNavigator::playNext(bool skipCurrent) {
        if (!animManager || !hasNext()) return false;
        
        if (skipCurrent && !animManager->empty()) {
            animManager->skipToEnd();
            animManager->setPaused(false);
        }
        
        // Manual navigation stops Auto-Play
        autoPlayEnabled = false;

        // 1. Advance to the next step index
        currentStepIndex++;

        // 2. Try replaying from history: 
        // To replay step X with animation, we must first restore state BEFORE step X (history[X])
        if (static_cast<int>(history.size()) > currentStepIndex) {
            if (snapshotRestorer) {
                snapshotRestorer(history[currentStepIndex]);
            }
            // Continue to part 4 to play the animation normally
        } else {
            // 3. First-time play: Save the state BEFORE playing
            if (snapshotProvider && static_cast<int>(history.size()) <= currentStepIndex) {
                history.push_back(snapshotProvider());
            }
        }

        // 4. Play the actual animation (Either first time or replay)
        if (currentStepIndex >= 0 && currentStepIndex < static_cast<int>(steps.size()) && steps[currentStepIndex]) {
            // Wrap in a proxy to share with AnimationManager
            animManager->addAnimation(std::make_unique<SharedAnimationProxy>(steps[currentStepIndex]));
        }
        
        return true;
    }

    bool StepNavigator::stepBack() {
        if (!animManager || currentStepIndex < 0) return false;

        // CRITICAL: If an animation is currently running, finish it instantly 
        // to ensure the snapshot we take for the 'After' state is correct.
        if (!animManager->empty()) {
            animManager->skipToEnd();
        }

        // If we are moving back from a step that hasn't had its 'After' state saved yet
        if (snapshotProvider && static_cast<int>(history.size()) == currentStepIndex + 1) {
            history.push_back(snapshotProvider());
        }

        autoPlayEnabled = false;
        animManager->clearAll();

        // Restore the state BEFORE Step X (history[X])
        if (snapshotRestorer && currentStepIndex >= 0 && currentStepIndex < (int)history.size()) {
            snapshotRestorer(history[currentStepIndex]);
            currentStepIndex--;
            return true;
        }

        return false;
    }

    bool StepNavigator::hasNext() const {
        return (currentStepIndex + 1 < static_cast<int>(steps.size()));
    }

    void StepNavigator::playAllRemaining() {
        if (!animManager || steps.empty()) return;

        // 1. If we are currently in the middle of history, restore the current state first
        if (currentStepIndex >= 0 && currentStepIndex < static_cast<int>(history.size())) {
            if (snapshotRestorer) {
                snapshotRestorer(history[currentStepIndex]);
            }
        }

        auto sequence = std::make_unique<SequenceAnimation>();
        while (hasNext()) {
            currentStepIndex++;
            sequence->add(std::make_unique<SharedAnimationProxy>(steps[currentStepIndex]));
        }
        animManager->addAnimation(std::move(sequence));
    }

    void StepNavigator::skipAll() {
        if (!animManager || steps.empty()) return;

        // 1. Interrupt any current auto-play and finish current animations instantly
        autoPlayEnabled = false;
        if (!animManager->empty()) {
            animManager->skipToEnd();
        }

        // 2. Process each step with "Smooth Landing" logic
        while (hasNext()) {
            // Check if we are approaching the end (last 2 steps)
            bool isNearEnd = (currentStepIndex + 3 >= static_cast<int>(steps.size()));

            if (isNearEnd) {
                // THE SMOOTH LANDING: Play the final steps normally
                // We pass 'false' to NOT skip the previous step, 
                // so they play one after another.
                playNext(false);
            } else {
                // Intermediate steps: Play and force to finish instantly
                playNext(true);
                animManager->skipToEnd();
            }
        }
        
        // 3. Ensure the final snapshot of the end state is captured 
        // (Note: playNext takes state BEFORE step, so we need one more for the AFTER state)
        if (snapshotProvider && !hasNext() && static_cast<int>(history.size()) == currentStepIndex + 1) {
            history.push_back(snapshotProvider());
        }
    }

} // namespace UI::Animations
