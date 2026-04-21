#pragma once

#include "Core/DSA/Trie.hpp"
#include "UI/DSA/Graph.hpp" 
#include "Core/AppContext.hpp"
#include "UI/Widgets/PseudoCodeViewer.hpp"
#include <string>
#include <unordered_map>

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

        void syncGraph();
        void triggerLayout(float duration = 0.5f);
        void submitAnimation(UI::Animations::AnimStepBuilder& b);

    public:
        TrieController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Trie& m,
                       UI::Widgets::PseudoCodeViewer* viewer = nullptr);

        void forceSnapLayout();
        void handleCreateRandom(int count);
        void handleCreateFromFile();
        void handleEditDataFile();
        void handleInsert(const std::string& word);
        void handleSearch(const std::string& word, bool isPrefix = false);
        void handleRemove(const std::string& word);
        void handleClearAll();
    };

} // namespace Controllers