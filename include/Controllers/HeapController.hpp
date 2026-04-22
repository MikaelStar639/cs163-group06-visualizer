#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/Heap.hpp"
#include "UI/DSA/Graph.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include <any>
#include <string>
#include <vector>

namespace UI::Animations { class AnimStepBuilder; struct HeapSnapshot; }

namespace Controllers {

    class HeapController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::Heap& model;
        UI::Widgets::PseudoCodeViewer* codeViewer = nullptr;

        float startX = 800.f; 
        float startY = 250.f;
        float spacing = 150.f;

        // Cemetery for nodes to prevent dangling pointers
        std::vector<std::unique_ptr<UI::DSA::Node>> masterNodePool;

        void syncGraphEdges();
        void triggerLayout(float duration = 0.5f);
        void submitAnimation(UI::Animations::AnimStepBuilder& b);

        std::any saveSnapshot();
        void restoreSnapshot(const std::any& s);
    public:
        HeapController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Heap& m, 
                       UI::Widgets::PseudoCodeViewer* viewer = nullptr);

        void forceSnapLayout();
        void forceVisualSync();
        
        // Creation and File I/O
        void handleCreateRandom(int size);
        void handleCreateFromFile();
        void handleEditDataFile();

        void handlePreHeapifiedRandom(int size);
        void handlePreHeapifiedFromFile();
        
        // Heap Specific Operations
        void handleInsert(int val);
        void handleRemoveRoot();
        void handleReturnRoot(); 
        void handleBuildHeap(const std::vector<int>& data);
        
        void handleClearAll();
    };

} // namespace Controllers