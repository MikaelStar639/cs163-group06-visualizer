#include "Controllers/TrieController.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "Core/Platform.hpp"
#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <random>

namespace Controllers {

    TrieController::TrieController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Trie& m,
                                   UI::Widgets::PseudoCodeViewer* viewer)
        : ctx(context), graph(g), model(m), codeViewer(viewer) 
    {
        ctx.stepNavigator.setSnapshotHandlers(
            [this]() { return this->saveSnapshot(); },
            [this](const std::any& s) { this->restoreSnapshot(s); }
        );

        if (graph.getNodes().empty()) {
            graph.addNode("Root", {startX, startY});
            poolToGraphMap[model.getRootIndex()] = 0;
        }
    }

    void TrieController::syncGraph() {
        const auto& pool = model.getPool();
        struct NodeInfo { int poolIdx; char c; };
        std::vector<NodeInfo> stack = { {model.getRootIndex(), ' '} };
        std::unordered_set<int> currentActive;
        
        while(!stack.empty()) {
            auto [pIdx, c] = stack.back(); stack.pop_back();
            currentActive.insert(pIdx);
            if (poolToGraphMap.find(pIdx) == poolToGraphMap.end()) {
                sf::Vector2f spawnPos = {startX, startY}; 
                std::string label = (c == ' ') ? "Root" : std::string(1, c);
                graph.addNode(label, spawnPos); 
                poolToGraphMap[pIdx] = graph.getNodes().size() - 1;
            }
            int gIdx = poolToGraphMap[pIdx];
            if (auto* n = graph.getNode(gIdx)) {
                n->setLabelColor(Config::UI::Colors::NodeText); // CẦN THIẾT: Reset màu chữ về trắng
                if (pool[pIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100)); 
                else n->setFillColor(Config::UI::Colors::NodeFill); 
            }
            for(int i=25; i>=0; --i) if(pool[pIdx].children[i] != -1) stack.push_back({pool[pIdx].children[i], (char)('a' + i)});
        }
        
        std::vector<int> toDeletePidx;
        for(auto const& [pIdx, gIdx] : poolToGraphMap) if (currentActive.find(pIdx) == currentActive.end()) toDeletePidx.push_back(pIdx);
        
        if (!toDeletePidx.empty()) {
            std::vector<std::pair<int, int>> deleteList;
            for(int pIdx : toDeletePidx) deleteList.push_back({poolToGraphMap[pIdx], pIdx});
            std::sort(deleteList.rbegin(), deleteList.rend());
            for(auto const& [gIdxDeleted, pIdx] : deleteList) {
                masterNodePool.push_back(graph.extractNode(gIdxDeleted));
                poolToGraphMap.erase(pIdx);
                for(auto& [p, g] : poolToGraphMap) if (g > gIdxDeleted) g--;
            }
        }
        syncEdges(false);
    }

    void TrieController::syncEdges(bool animate) {
        if (animate) graph.clearEdges();
        else graph.clearEdgesSilently();

        const auto& pool = model.getPool();
        for(auto const& [pIdx, gIdx] : poolToGraphMap) {
            // Safety check: ensure pIdx is valid in the current pool to prevent OOB crashes during snapshot restore
            if (pIdx < 0 || pIdx >= (int)pool.size()) continue;

            for(int i=0; i<26; ++i) {
                int childPidx = pool[pIdx].children[i];
                if (childPidx != -1 && poolToGraphMap.count(childPidx)) {
                    int destGidx = poolToGraphMap[childPidx];
                    graph.addEdge(gIdx, destGidx, "", animate);
                }
            }
        }
    }

    void TrieController::triggerLayout(float duration) {
        auto layoutAnim = UI::DSA::LayoutEngine::asTrie(graph, model, poolToGraphMap, startX, startY, horizontalSpacing, verticalSpacing, duration);
        if (duration <= 0.f) layoutAnim->update(9999.f);
        else ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void TrieController::triggerLayoutWithModel(const Core::DSA::Trie& layoutModel, float duration) {
        auto layoutAnim = UI::DSA::LayoutEngine::asTrie(graph, layoutModel, poolToGraphMap, startX, startY, horizontalSpacing, verticalSpacing, duration);
        if (duration <= 0.f) layoutAnim->update(9999.f);
        else ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void TrieController::submitAnimation(UI::Animations::AnimStepBuilder& b) {
        ctx.stepNavigator.forceFinishAll(); // Instantly finish previous operation before starting new one
        ctx.animManager.clearAll(); // Ensure queue is clean
        ctx.stepNavigator.clear();
        masterNodePool.clear();
        graph.resetVisuals(); // Ensure no leftover highlights from interrupted operations
        auto steps = b.buildSteps();
        for (auto& step : steps) ctx.stepNavigator.addStep(std::move(step));
        ctx.stepNavigator.playNext();
        if (ctx.isStepByStep) ctx.animManager.setPaused(true);
    }

    void TrieController::forceSnapLayout() {
        syncGraph(); 
        std::unordered_map<int, sf::Vector2f> targetPositions;
        const auto& pool = model.getPool();
        int rootIdx = model.getRootIndex();
        float currentLeafX = 0.f; 
        auto calculateDFS = [&](auto& self, int poolIdx, int depth) -> float {
            std::vector<int> children;
            for (int i = 0; i < 26; ++i) if (pool[poolIdx].children[i] != -1) children.push_back(pool[poolIdx].children[i]);
            float myX = 0.f;
            if (children.empty()) { myX = currentLeafX; currentLeafX += horizontalSpacing; }
            else { float sumX = 0; for (int child : children) sumX += self(self, child, depth + 1); myX = sumX / children.size(); }
            targetPositions[poolIdx] = {myX, startY + depth * verticalSpacing};
            return myX;
        };
        if (rootIdx != -1) {
            calculateDFS(calculateDFS, rootIdx, 0);
            float offsetX = startX - targetPositions[rootIdx].x;
            for (auto& pair : targetPositions) {
                pair.second.x += offsetX;
                auto it = poolToGraphMap.find(pair.first);
                if (it != poolToGraphMap.end()) if (auto* node = graph.getNode(it->second)) node->setPosition(pair.second); 
            }
        }
    }

    void TrieController::handleCreateRandom(int count) {
        if (codeViewer) codeViewer->hide();
        handleClearAll();

        // Safety limit for visualizer (increased to 30 as requested)
        if (count > 30) count = 30;

        std::vector<std::string> randomWords;
        for (int i = 0; i < count; ++i) {
            int len = 3 + rand() % 3; 
            std::string word = "";
            for (int j = 0; j < len; ++j) word += (char)('a' + rand() % 26);
            if (std::find(randomWords.begin(), randomWords.end(), word) == randomWords.end()) {
                randomWords.push_back(word); model.insert(word); 
            }
        }
        syncGraph(); triggerLayout(Config::Animation::DURATION_LAYOUT);
        ctx.animManager.setPaused(false);
    }

    void TrieController::handleEditDataFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/TrieData.txt";
        if (!std::filesystem::exists(dirPath)) std::filesystem::create_directories(dirPath);
        std::string header = "# --- TRIE VISUALIZER DATA ---\n"
                             "# DETAILED INSTRUCTIONS:\n"
                             "# 1. Type the number of words 'n' first.\n"
                             "# 2. Then type the 'n' words separated by spaces or newlines.\n"
                             "#    (Max n is 30. Words should only contain letters).\n"
                             "# 3. Do NOT use commas (,) or other punctuation marks.\n"
                             "# 4. When you are done:\n"
                             "#    - Save this file by pressing Ctrl + S\n"
                             "#    - Go back to the Application and click the 'Go' button.\n"
                             "# -----------------------------------\n";
        std::ifstream inFile(filePath);
        std::string userContent = "";
        if (inFile.is_open()) {
            std::string line;
            while (std::getline(inFile, line)) {
                size_t startPos = line.find_first_not_of(" \t\r\n");
                if (startPos != std::string::npos && line[startPos] != '#') userContent += line + "\n";
            }
            inFile.close();
        }
        std::ofstream outFile(filePath);
        if (outFile.is_open()) { outFile << header << userContent; outFile.close(); }
        Core::Platform::openTextEditor(filePath);
    }

    void TrieController::handleCreateFromFile() {
        if (codeViewer) codeViewer->hide();
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/TrieData.txt";
        if (!std::filesystem::exists(dirPath)) { handleEditDataFile(); return; }
        
        std::ifstream file(filePath);
        std::string line;
        std::vector<std::string> rawTokens;
        std::string originalDataLines = "";

        while (std::getline(file, line)) {
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos != std::string::npos && line[startPos] == '#') continue;
            if (startPos != std::string::npos) originalDataLines += line + "\n";

            std::stringstream ss(line);
            std::string token;
            while (ss >> token) rawTokens.push_back(token);
        }
        file.close();

        std::string errorMsg = "";
        int n = -1;

        if (rawTokens.empty()) {
            errorMsg = "# [WARNING] File is empty. Please enter 'n' followed by words.\n";
        } else {
            try {
                n = std::stoi(rawTokens[0]);
                if (n < 0 || n > 30) {
                    errorMsg = "# [WARNING] Size 'n' = " + std::to_string(n) + " is invalid (Max 30).\n";
                } else if ((int)rawTokens.size() - 1 < n) {
                    errorMsg = "# [WARNING] Expected " + std::to_string(n) + " words, found " + std::to_string(rawTokens.size() - 1) + ".\n";
                }
            } catch (...) {
                errorMsg = "# [WARNING] First value must be an integer 'n'.\n";
            }
        }

        if (!errorMsg.empty()) {
            std::cout << "[UI LOG] Data error. Opening Notepad to fix.\n";
            std::string header = "# --- TRIE VISUALIZER DATA ---\n"
                                 "# DETAILED INSTRUCTIONS:\n"
                                 "# 1. Type the number of words 'n' first.\n"
                                 "# 2. Then type the 'n' words separated by spaces or newlines.\n"
                                 "#    (Max n is 30. Words should only contain letters).\n"
                                 "# 3. Do NOT use commas (,) or other punctuation marks.\n"
                                 "# 4. When you are done:\n"
                                 "#    - Save this file by pressing Ctrl + S\n"
                                 "#    - Go back to the Application and click the 'Go' button.\n"
                                 "# -----------------------------------\n";
            std::string contentWithWarning = header + errorMsg + originalDataLines;
            std::ofstream outFileErr(filePath);
            if (outFileErr.is_open()) {
                outFileErr << contentWithWarning;
                outFileErr.close();
            }
            Core::Platform::openTextEditor(filePath);
            return;
        }

        handleClearAll(); 
        for (int i = 1; i <= n && i < (int)rawTokens.size(); ++i) {
            model.insert(sanitize(rawTokens[i]));
        }
        syncGraph(); triggerLayout(Config::Animation::DURATION_LAYOUT);  
        ctx.animManager.setPaused(false);
    }

    std::string TrieController::sanitize(const std::string& word) {
        std::string res = "";
        for (char c : word) {
            if (std::isalpha(c)) res += (char)std::tolower(c);
        }
        return res;
    }

    void TrieController::handleInsert(std::string word) {
        word = sanitize(word);
        if (word.empty()) return;

        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::insert();
        Builder b(codeDef, codeViewer);

        int tempCurr = model.getRootIndex();
        int existingChars = 0;
        for (char c : word) {
            int nextIdx = model.getPool()[tempCurr].children[c - 'a'];
            if (nextIdx != -1) { existingChars++; tempCurr = nextIdx; }
            else break;
        }

        Core::DSA::Trie futureModel = model;
        futureModel.insert(word); 

        b.highlight("init_curr").callback([this, word]() {
            // Delay updating the real model until the first step plays.
            // This ensures Snapshot 0 (captured before Step 0) contains the OLD model.
            this->model.insert(word); 
        }).nextStep();
        int currPoolIdx = futureModel.getRootIndex();
        
        for (int i = 0; i < (int)word.length(); ++i) {
            char c = word[i];
            int nextPoolIdx = futureModel.getPool()[currPoolIdx].children[c - 'a'];

            b.highlight("loop_char").callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                    n->setFillColor(Config::UI::Colors::NodeHighlight); n->setLabelColor(Config::UI::Colors::LabelHighlight);
                }
            }).wait(Config::Animation::STEP_WAIT_TRAVERSAL);
            
            b.highlight("check_null").nextStep();
            
            if (i >= existingChars) {
                b.highlight("create_node").callback([this, futureModel, currPoolIdx, nextPoolIdx, c]() {
                    if (!poolToGraphMap.count(currPoolIdx)) return; 
                    int parentUiIdx = poolToGraphMap[currPoolIdx];
                    auto* parentNode = graph.getNode(parentUiIdx);
                    if (!parentNode) return; 
                    graph.addNode(std::string(1, c), parentNode->getPosition()); 
                    int newUiIdx = (int)graph.getNodes().size() - 1;
                    poolToGraphMap[nextPoolIdx] = newUiIdx;
                    graph.addEdge(parentUiIdx, newUiIdx, "");

                    triggerLayoutWithModel(futureModel, Config::Animation::DURATION_LAYOUT);
                }).wait(Config::Animation::STEP_WAIT_LAYOUT).nextStep();
            }
            
            b.highlight("advance").callback([this, futureModel, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                    n->setLabelColor(Config::UI::Colors::NodeText);
                    // CRITICAL: Use futureModel for state checks during traversal to avoid OOB on current model
                    if (futureModel.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100)); 
                    else n->setFillColor(Config::UI::Colors::NodeFill); 
                }
            }).wait(Config::Animation::STEP_WAIT_TRAVERSAL).nextStep();
            currPoolIdx = nextPoolIdx;
        }

        b.highlight("set_end").nextStep();
        b.callback([this, currPoolIdx]() {
            if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                n->setFillColor(Config::UI::Colors::NodeHighlight); n->setLabelColor(Config::UI::Colors::LabelHighlight);
            }
        }).wait(Config::Animation::DURATION_COLOR).callback([this, futureModel, currPoolIdx]() {
            if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                n->setFillColor(sf::Color(70, 160, 100)); n->setLabelColor(Config::UI::Colors::NodeText);
            }
            triggerLayoutWithModel(futureModel, Config::Animation::DURATION_LAYOUT);
        }).wait(Config::Animation::STEP_WAIT_LAYOUT).nextStep();

        // FINAL CLEANUP: Ensure everything is unhighlighted
        b.callback([this]() { this->syncGraph(); }).finish(); 
        submitAnimation(b);
    }

    void TrieController::handleSearch(std::string word, bool isPrefix) {
        word = sanitize(word);
        if (word.empty()) return;

        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::search();
        Builder b(codeDef, codeViewer);
        b.highlight("init_curr").nextStep();
        int currPoolIdx = model.getRootIndex();
        bool broken = false;
        for (char c : word) {
            b.highlight("loop_char").nextStep();
            int nextPoolIdx = model.getPool()[currPoolIdx].children[c - 'a'];
            b.callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                    n->setFillColor(Config::UI::Colors::NodeHighlight); n->setLabelColor(Config::UI::Colors::LabelHighlight);
                }
            }).wait(Config::Animation::STEP_WAIT_TRAVERSAL);
            b.highlight("check_null").nextStep();
            if (nextPoolIdx == -1) {
                b.highlight("not_found").nextStep();
                b.callback([this, currPoolIdx]() {
                    if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText);
                        if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                        else n->setFillColor(Config::UI::Colors::NodeFill);
                    }
                });
                broken = true; break;
            }
            b.highlight("advance").callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                    n->setLabelColor(Config::UI::Colors::NodeText);
                    if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                    else n->setFillColor(Config::UI::Colors::NodeFill);
                }
            }).nextStep();
            currPoolIdx = nextPoolIdx; 
        }
        if (!broken) {
            b.highlight("check_end").nextStep();
            if (model.getPool()[currPoolIdx].isEndOfWord || isPrefix) {
                b.highlight("found").callback([this, currPoolIdx]() {
                    if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) n->setFillColor(Config::UI::Colors::NodeHighlight);
                }).wait(Config::Animation::STEP_WAIT_ACTION).nextStep();
            } else b.highlight("not_found_end").nextStep();
        }
        
        // FINAL CLEANUP: Ensure everything is unhighlighted
        b.callback([this]() { this->syncGraph(); }).finish(); 
        submitAnimation(b);
    }

    void TrieController::handleRemove(std::string word) {
        word = sanitize(word);
        if (word.empty()) return;

        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::deleteWord();
        Builder b(codeDef, codeViewer);
        b.highlight("init_curr").nextStep(); 
        int currPoolIdx = model.getRootIndex();
        bool found = true;

        for (char c : word) {
            b.highlight("loop_char").nextStep(); 
            int nextPoolIdx = model.getPool()[currPoolIdx].children[c - 'a'];
            b.callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) { 
                    n->setFillColor(Config::UI::Colors::NodeHighlight); n->setLabelColor(Config::UI::Colors::LabelHighlight); 
                }
            }).wait(Config::Animation::STEP_WAIT_TRAVERSAL);
            
            b.highlight("check_null").nextStep(); 
            if (nextPoolIdx == -1) { 
                b.highlight("not_found").nextStep(); 
                found = false;
                break; 
            }

            b.highlight("advance").callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) { 
                    n->setLabelColor(Config::UI::Colors::NodeText); 
                    if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100)); 
                    else n->setFillColor(Config::UI::Colors::NodeFill); 
                }
            }).nextStep();
            currPoolIdx = nextPoolIdx;
        }

        if (found) {
            if (model.getPool()[currPoolIdx].isEndOfWord) {
                b.highlight("delete").nextStep().callback([this, word]() { 
                    model.deleteWord(word); 
                    syncGraph(); 
                    triggerLayout(Config::Animation::DURATION_LAYOUT); 
                }).nextStep();
            } else {
                b.highlight("not_found").nextStep();
            }
        }

        // FINAL CLEANUP: Ensure everything is unhighlighted
        b.callback([this]() { this->syncGraph(); }).finish();
        submitAnimation(b);
    }
    
    void TrieController::handleClearAll() {
        if (codeViewer) codeViewer->hide();
        ctx.animManager.clearAll(); ctx.stepNavigator.clear(); masterNodePool.clear();
        model.clear(); graph.clear(); poolToGraphMap.clear();
        graph.addNode("Root", {startX, startY}); poolToGraphMap[model.getRootIndex()] = 0; 
        triggerLayout(0.f); 
        ctx.animManager.setPaused(false);
    }

    std::any TrieController::saveSnapshot() {
        // CRITICAL: Force all nodes to their intended layout positions (0 duration)
        // This ensures the snapshot captures perfect coordinates, not interpolated ones during movement.
        triggerLayout(0.f);

        UI::Animations::TrieSnapshot s; s.trieModel = model; s.poolToGraphMap = poolToGraphMap;
        for (const auto& nodePtr : graph.getNodes()) {
            UI::Animations::TrieSnapshot::NodeState ns;
            ns.originalPointer = nodePtr.get(); ns.position = nodePtr->getPosition();
            ns.fillColor = nodePtr->getFillColor(); ns.outlineColor = nodePtr->getOutlineColor();
            ns.labelColor = nodePtr->getLabelColor(); ns.scale = nodePtr->getScale(); ns.label = nodePtr->getLabel();
            s.nodes.push_back(ns);
        }
        if (codeViewer) s.activeLineIndex = codeViewer->getActiveLine();
        return std::make_any<UI::Animations::TrieSnapshot>(std::move(s));
    }

    void TrieController::restoreSnapshot(const std::any& snapshotAny) {
        const auto& s = std::any_cast<const UI::Animations::TrieSnapshot&>(snapshotAny);
        
        // CRITICAL: Stop any pending layout or visual animations that might fight the snapshot state
        ctx.animManager.clearAll();

        model = s.trieModel; poolToGraphMap = s.poolToGraphMap;
        while (graph.getNodes().size() > 0) masterNodePool.push_back(graph.extractNode(0));
        for (const auto& ns : s.nodes) {
            auto it = std::find_if(masterNodePool.begin(), masterNodePool.end(), [&](const std::unique_ptr<UI::DSA::Node>& n) { return n.get() == ns.originalPointer; });
            if (it != masterNodePool.end()) {
                std::unique_ptr<UI::DSA::Node> node = std::move(*it); masterNodePool.erase(it);
                node->setLabel(ns.label); node->setPosition(ns.position);
                node->setFillColor(ns.fillColor); node->setOutlineColor(ns.outlineColor);
                node->setLabelColor(ns.labelColor); node->setScale(ns.scale);
                graph.insertNodePtr(-1, std::move(node));
            } else {
                auto* newNode = graph.addNodeRaw(ns.label, ns.position);
                newNode->setFillColor(ns.fillColor); newNode->setOutlineColor(ns.outlineColor);
                newNode->setLabelColor(ns.labelColor); newNode->setScale(ns.scale);
            }
        }
        syncEdges(false);    // SILENT restore edges based on current snapped positions
        if (codeViewer) { if (s.activeLineIndex >= 0) codeViewer->highlightLine(s.activeLineIndex); else codeViewer->clearHighlight(); }
    }

} // namespace Controllers
