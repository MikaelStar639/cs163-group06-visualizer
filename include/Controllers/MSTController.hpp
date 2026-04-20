#pragma once

#include "Core/AppContext.hpp"
#include "Core/DSA/MST.hpp"
#include "UI/DSA/Graph.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include <tuple>
#include <vector>
#include <string>

namespace Controllers {

    class MSTController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::MST& model;
        UI::Widgets::PseudoCodeViewer* codeViewer = nullptr;

        float centerX = 820.f;
        float centerY = 430.f;
        float radius  = 260.f;

        void rebuildGraphFromModel();
        void triggerCircularLayout(float duration = 0.5f);
        UI::DSA::Edge* getEdgeById(int edgeId) const;

        int lastTotalWeight = 0;
        int lastSelectedEdgeCount = 0;
        std::string lastAlgorithm = "None";

        std::string liveMessage = "Idle";
        int liveTotalWeight = 0;
        int liveSelectedEdgeCount = 0;
        bool running = false;

    public:
        MSTController(AppContext& context,
                      UI::DSA::Graph& g,
                      Core::DSA::MST& m,
                      UI::Widgets::PseudoCodeViewer* viewer = nullptr);

        void forceSnapLayout();

        void handleCreateRandom(int nodeCount, int edgeCount);
        void handleCreateFromFile();
        void handleEditDataFile();
        void handleCreateManual(int nodeCount,
                                const std::vector<int>& nodeValues,
                                const std::vector<std::tuple<int,int,int>>& rawEdges);

        void handleRunKruskal();
        void handleRunPrim(int startNode);

        void handleClearAll();

        int getLastTotalWeight() const { return lastTotalWeight; }
        int getLastSelectedEdgeCount() const { return lastSelectedEdgeCount; }
        const std::string& getLastAlgorithm() const { return lastAlgorithm; }

        const std::string& getLiveMessage() const { return liveMessage; }
        int getLiveTotalWeight() const { return liveTotalWeight; }
        int getLiveSelectedEdgeCount() const { return liveSelectedEdgeCount; }
        bool isRunning() const { return running; }

        void interruptRunning(const std::string& reason = "Graph edited");
    };

} // namespace Controllers