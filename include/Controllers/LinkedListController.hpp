#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/LinkedList.hpp"
#include "UI/DSA/Graph.hpp"
#include <string>

namespace Controllers {

    class LinkedListController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::LinkedList& model;

        float startX = 100.f;
        float startY = 400.f;
        float spacing = 150.f;

        void syncGraphEdges();
        void triggerLayout(float duration = 0.5f);

    public:
        LinkedListController(AppContext& context, UI::DSA::Graph& g, Core::DSA::LinkedList& m);

        void handleCreateRandom(int size);
        void handleInsert(int sel, int pos, int val);
        void handleRemove(int sel, int pos);
        void handleSearch(int targetValue);
        void handleUpdate(int sel, int pos, int oldVal, int newVal);
        void handleClearAll();
    };

} // namespace Controllers