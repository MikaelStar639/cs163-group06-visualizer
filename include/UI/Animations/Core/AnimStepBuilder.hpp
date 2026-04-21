#pragma once
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Core/WaitAnimation.hpp"
#include "UI/Animations/Core/ParallelAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Node/NodeSwapAnimation.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include <memory>
#include <functional>

namespace UI::Animations {

    // Builder pattern for constructing animation sequences synced with pseudocode.
    // Eliminates magic line numbers and repetitive boilerplate in controllers.
    class AnimStepBuilder {
    private:
        std::unique_ptr<SequenceAnimation> currentSequence;
        std::vector<std::unique_ptr<AnimationBase>> steps;
        UI::Widgets::PseudoCodeViewer* viewer;
        Core::DSA::PseudoCodeDef codeDef;

    public:
        AnimStepBuilder(const Core::DSA::PseudoCodeDef& def, UI::Widgets::PseudoCodeViewer* viewer);

        // --- Pseudocode highlight ---

        // Highlight a line by its label
        AnimStepBuilder& highlight(const std::string& label);

        // Highlight a line by index (fallback for edge cases)
        AnimStepBuilder& highlightIndex(int index);

        // --- Timing ---

        // Wait for a base duration (will be scaled by TimeManager when integrated)
        AnimStepBuilder& wait(float baseDuration = 0.4f);

        // --- Node animations ---

        AnimStepBuilder& nodeHighlight(UI::DSA::Node* node, float duration = 0.3f);
        AnimStepBuilder& nodeUnhighlight(UI::DSA::Node* node, float duration = 0.1f);
        AnimStepBuilder& nodeScale(UI::DSA::Node* node, float from, float to, float duration = 0.2f);
        AnimStepBuilder& nodeSwap(UI::DSA::Node* a, UI::DSA::Node* b, float duration);

        AnimStepBuilder& nodesHighlight(const std::vector<UI::DSA::Node*>& nodes, float duration);
        AnimStepBuilder& nodesUnhighlight(const std::vector<UI::DSA::Node*>& nodes, float duration);
        // --- Callbacks ---

        AnimStepBuilder& callback(std::function<void()> fn);

        // --- Finalization ---

        // Add final wait + hide pseudocode viewer
        AnimStepBuilder& finish(float finalWait = 0.5f);

        // Finalize current step and start a new one
        AnimStepBuilder& nextStep();

        // Release the built sequence (single monolithic animation)
        std::unique_ptr<SequenceAnimation> build();

        // Release the built steps (multiple logical blocks)
        std::vector<std::unique_ptr<AnimationBase>> buildSteps();
    };

} // namespace UI::Animations
