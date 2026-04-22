#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include "UI/Animations/Edge/EdgeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include <iostream>

namespace UI::Animations {

    AnimStepBuilder::AnimStepBuilder(const Core::DSA::PseudoCodeDef& def, UI::Widgets::PseudoCodeViewer* viewer)
        : currentSequence(std::make_unique<SequenceAnimation>()), viewer(viewer), codeDef(def)
    {
        // Automatically set the code on the viewer when builder is created
        if (viewer) {
            viewer->setCode(codeDef);
        }
    }

    AnimStepBuilder& AnimStepBuilder::highlight(const std::string& label) {
        int index = codeDef.lineIndex(label);
        if (index < 0) {
            std::cerr << "[AnimStepBuilder] WARNING: label '" << label << "' not found in '" << codeDef.name << "'" << std::endl;
            return *this;
        }
        return highlightIndex(index);
    }

    AnimStepBuilder& AnimStepBuilder::highlightIndex(int index) {
        if (!viewer) return *this;
        auto* v = viewer;
        currentSequence->add(std::make_unique<CallbackAnimation>([v, index]() {
            v->highlightLine(index);
        }));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::wait(float baseDuration) {
        currentSequence->add(std::make_unique<WaitAnimation>(baseDuration));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeHighlight(UI::DSA::Node* node, float duration) {
        if (node) {
            currentSequence->add(std::make_unique<NodeHighlightAnimation>(node, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeUnhighlight(UI::DSA::Node* node, float duration) {
        if (node) {
            currentSequence->add(std::make_unique<NodeUnhighlightAnimation>(node, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeScale(UI::DSA::Node* node, float from, float to, float duration) {
        if (node) {
            currentSequence->add(std::make_unique<NodeScaleAnimation>(node, from, to, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeSwap(UI::DSA::Node* a, UI::DSA::Node* b, float duration) {
        if (a && b) {
            currentSequence->add(std::make_unique<NodeSwapAnimation>(a, b, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodesHighlight(const std::vector<UI::DSA::Node*>& nodes, float duration) {
        auto parallel = std::make_unique<ParallelAnimation>();

        for (auto* node : nodes) {
            if (node) {
                parallel->add(std::make_unique<NodeHighlightAnimation>(node, duration));
            }
        }

        currentSequence->add(std::move(parallel));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodesUnhighlight(const std::vector<UI::DSA::Node*>& nodes, float duration) {
        auto parallel = std::make_unique<ParallelAnimation>();

        for (auto* node : nodes) {
            if (node) {
                parallel->add(std::make_unique<NodeUnhighlightAnimation>(node, duration));
            }
        }

        currentSequence->add(std::move(parallel));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeColor(UI::DSA::Node* node, sf::Color fill, sf::Color text, float duration) {
        if (node) {
            currentSequence->add(std::make_unique<NodeColorAnimation>(node, fill, text, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::edgeColor(UI::DSA::Edge* edge, sf::Color from, sf::Color to, float duration) {
        if (edge) {
            currentSequence->add(std::make_unique<EdgeColorAnimation>(edge, from, to, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::edgeScale(UI::DSA::Edge* edge, float from, float to, float duration) {
        if (edge) {
            currentSequence->add(std::make_unique<EdgeScaleAnimation>(edge, from, to, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::callback(std::function<void()> fn) {
        currentSequence->add(std::make_unique<CallbackAnimation>(std::move(fn)));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::finish(float finalWait) {
        if (viewer) {
            auto* v = viewer;
            currentSequence->add(std::make_unique<WaitAnimation>(finalWait));
            currentSequence->add(std::make_unique<CallbackAnimation>([v]() {
                v->hide();
            }));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nextStep() {
        if (!currentSequence->isEmpty()) {
            steps.push_back(std::move(currentSequence));
            currentSequence = std::make_unique<SequenceAnimation>();
        }
        return *this;
    }

    std::unique_ptr<SequenceAnimation> AnimStepBuilder::build() {
        if (steps.empty()) return std::move(currentSequence);
        
        // If we have steps, combine them into one sequence
        auto fullSequence = std::make_unique<SequenceAnimation>();
        for (auto& step : steps) {
            fullSequence->add(std::move(step));
        }
        if (!currentSequence->isEmpty()) {
             fullSequence->add(std::move(currentSequence));
        }
        return fullSequence;
    }

    std::vector<std::unique_ptr<AnimationBase>> AnimStepBuilder::buildSteps() {
        if (!currentSequence->isEmpty()) {
            steps.push_back(std::move(currentSequence));
            currentSequence = std::make_unique<SequenceAnimation>();
        }
        return std::move(steps);
    }

} // namespace UI::Animations
