#include "UI/Animations/StepByStep/StepNavigator.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"

namespace UI::Animations {

    void StepNavigator::clear() {
        steps.clear();
        currentStepIndex = -1;
    }

    void StepNavigator::addStep(std::unique_ptr<AnimationBase> step) {
        if (step) {
            steps.push_back(std::move(step));
        }
    }

    void StepNavigator::update(float dt) {
        if (!stepModeActive && hasNext() && animManager && animManager->empty()) {
            playNext();
        }
    }

    bool StepNavigator::playNext() {
        if (!animManager) return false;
        
        // If there's an animation already running/paused, finish it instantly
        // before starting the next logical step to ensure we don't 'jam'.
        if (!animManager->empty()) {
            animManager->skipToEnd();
            animManager->setPaused(false);
        }

        if (currentStepIndex + 1 < static_cast<int>(steps.size())) {
            currentStepIndex++;
            animManager->addAnimation(std::move(steps[currentStepIndex]));
            return true;
        }

        return false;
    }

    bool StepNavigator::hasNext() const {
        return (currentStepIndex + 1 < static_cast<int>(steps.size()));
    }

    void StepNavigator::playAllRemaining() {
        if (!animManager || !hasNext()) return;

        auto sequence = std::make_unique<SequenceAnimation>();
        while (hasNext()) {
            currentStepIndex++;
            sequence->add(std::move(steps[currentStepIndex]));
        }
        animManager->addAnimation(std::move(sequence));
    }

    void StepNavigator::skipAll() {
        if (!animManager) return;

        // Play remaining steps through the manager
        while (currentStepIndex + 1 < static_cast<int>(steps.size())) {
            currentStepIndex++;
            animManager->addAnimation(std::move(steps[currentStepIndex]));
        }
        
        // Tell manager to finish everything instantly
        animManager->skipToEnd();
    }

} // namespace UI::Animations
