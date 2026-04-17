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
        std::unique_ptr<SequenceAnimation> sequence;
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

        AnimStepBuilder& nodesHighlight(UI::DSA::Node* a, UI::DSA::Node* b, float duration);
        AnimStepBuilder& nodesUnhighlight(UI::DSA::Node* a, UI::DSA::Node* b, float duration);
        // --- Callbacks ---

        AnimStepBuilder& callback(std::function<void()> fn);

        // --- Finalization ---

        // Add final wait + hide pseudocode viewer
        AnimStepBuilder& finish(float finalWait = 0.5f);

        // Release the built sequence for use with AnimationManager
        std::unique_ptr<SequenceAnimation> build();
    };

} // namespace UI::Animations
