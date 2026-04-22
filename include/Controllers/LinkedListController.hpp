#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/LinkedList.hpp"
#include "UI/DSA/Graph.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include "UI/Animations/StepByStep/LinkedListSnapshot.hpp"
#include <any>
#include <string>

namespace Controllers {

    class LinkedListController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::LinkedList& model;
        UI::Widgets::PseudoCodeViewer* codeViewer = nullptr;

        float startX = 100.f;
        float startY = 400.f;
        float spacing = 150.f;
        
        // Keeps all nodes created during the session alive to prevent dangling pointers in animations
        std::vector<std::unique_ptr<UI::DSA::Node>> masterNodePool;

        void syncGraphEdges(bool animate = true);
        void triggerLayout(float duration = 0.5f);
        void submitAnimation(UI::Animations::AnimStepBuilder& b);

        // Snapshot methods for Back feature
        std::any saveSnapshot();
        void restoreSnapshot(const std::any& s);

    public:
        LinkedListController(AppContext& context, UI::DSA::Graph& g, Core::DSA::LinkedList& m,
                             UI::Widgets::PseudoCodeViewer* viewer = nullptr);

        void forceSnapLayout();
        void handleCreateRandom(int size);
        void handleCreateFromFile();
        void handleEditDataFile();
        void handleInsert(int sel, int pos, int val);
        void handleRemove(int sel, int pos);
        void handleSearch(int targetValue);
        void handleUpdate(int sel, int pos, int oldVal, int newVal);
        void handleClearAll();
    };

} // namespace Controllers