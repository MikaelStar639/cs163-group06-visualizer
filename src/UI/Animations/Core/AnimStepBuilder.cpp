#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include <iostream>

namespace UI::Animations {

    AnimStepBuilder::AnimStepBuilder(const Core::DSA::PseudoCodeDef& def, UI::Widgets::PseudoCodeViewer* viewer)
        : sequence(std::make_unique<SequenceAnimation>()), viewer(viewer), codeDef(def)
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
        sequence->add(std::make_unique<CallbackAnimation>([v, index]() {
            v->highlightLine(index);
        }));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::wait(float baseDuration) {
        sequence->add(std::make_unique<WaitAnimation>(baseDuration));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeHighlight(UI::DSA::Node* node, float duration) {
        if (node) {
            sequence->add(std::make_unique<NodeHighlightAnimation>(node, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeUnhighlight(UI::DSA::Node* node, float duration) {
        if (node) {
            sequence->add(std::make_unique<NodeUnhighlightAnimation>(node, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeScale(UI::DSA::Node* node, float from, float to, float duration) {
        if (node) {
            sequence->add(std::make_unique<NodeScaleAnimation>(node, from, to, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodeSwap(UI::DSA::Node* a, UI::DSA::Node* b, float duration) {
        if (a && b) {
            sequence->add(std::make_unique<NodeSwapAnimation>(a, b, duration));
        }
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodesHighlight(UI::DSA::Node* a, UI::DSA::Node* b, float duration) {
        auto parallel = std::make_unique<ParallelAnimation>();
        
        if (a) parallel->add(std::make_unique<NodeHighlightAnimation>(a, duration));
        if (b) parallel->add(std::make_unique<NodeHighlightAnimation>(b, duration));
        
        // By adding the parallel object to the sequence, 
        // both highlights will start and finish at the same time.
        sequence->add(std::move(parallel));
        
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::nodesUnhighlight(UI::DSA::Node* a, UI::DSA::Node* b, float duration) {
        auto parallel = std::make_unique<ParallelAnimation>();
        
        if (a) parallel->add(std::make_unique<NodeUnhighlightAnimation>(a, duration));
        if (b) parallel->add(std::make_unique<NodeUnhighlightAnimation>(b, duration));
        
        sequence->add(std::move(parallel));
        
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::callback(std::function<void()> fn) {
        sequence->add(std::make_unique<CallbackAnimation>(std::move(fn)));
        return *this;
    }

    AnimStepBuilder& AnimStepBuilder::finish(float finalWait) {
        if (viewer) {
            auto* v = viewer;
            sequence->add(std::make_unique<WaitAnimation>(finalWait));
            sequence->add(std::make_unique<CallbackAnimation>([v]() {
                v->hide();
            }));
        }
        return *this;
    }

    std::unique_ptr<SequenceAnimation> AnimStepBuilder::build() {
        return std::move(sequence);
    }

} // namespace UI::Animations
