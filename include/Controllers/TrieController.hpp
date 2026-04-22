#pragma once

#include "Core/DSA/Trie.hpp"
#include "UI/DSA/Graph.hpp" 
#include "Core/AppContext.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include <string>
#include <unordered_map>
#include <any>
#include "UI/Animations/StepByStep/TrieSnapshot.hpp"

namespace UI::Animations { class AnimStepBuilder; }

namespace Controllers {

    class TrieController {
    private:
        AppContext& ctx;
        UI::DSA::Graph& graph;
        Core::DSA::Trie& model;
        UI::Widgets::PseudoCodeViewer* codeViewer;

        float startX = 800.f;
        float startY = 300.f;
        float verticalSpacing = 130.f;
        float horizontalSpacing = 100.f;

        std::unordered_map<int, int> poolToGraphMap;
        std::vector<std::unique_ptr<UI::DSA::Node>> masterNodePool;

        void syncGraph();
        void syncEdges(bool animate = true);
        void triggerLayout(float duration = 0.5f);
        void triggerLayoutWithModel(const Core::DSA::Trie& layoutModel, float duration);
        void submitAnimation(UI::Animations::AnimStepBuilder& b);

        std::string sanitize(const std::string& word);
        std::any saveSnapshot();
        void restoreSnapshot(const std::any& snapshotAny);

    public:
        TrieController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Trie& m,
                       UI::Widgets::PseudoCodeViewer* viewer = nullptr);

        void forceSnapLayout();
        void handleCreateRandom(int count);
        void handleCreateFromFile();
        void handleEditDataFile();
        void handleInsert(std::string word);
        void handleSearch(std::string word, bool isPrefix = false);
        void handleRemove(std::string word);
        void handleClearAll();
    };

} // namespace Controllers