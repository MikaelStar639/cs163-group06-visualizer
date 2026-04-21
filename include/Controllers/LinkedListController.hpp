#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/LinkedList.hpp"
#include "UI/DSA/Graph.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
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

        void syncGraphEdges();
        void triggerLayout(float duration = 0.5f);
        void submitAnimation(UI::Animations::AnimStepBuilder& b);

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