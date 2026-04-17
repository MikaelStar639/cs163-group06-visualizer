#include "Controllers/TrieController.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
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
        
        // DFS: Add Node and colors update
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
                if (pool[pIdx].isEndOfWord) {
                    n->setFillColor(sf::Color(70, 160, 100)); 
                } else {
                    n->setFillColor(Config::UI::Colors::NodeFill); 
                }
            }
            
            for(int i=25; i>=0; --i){
                if(pool[pIdx].children[i] != -1) {
                    stack.push_back({pool[pIdx].children[i], (char)('a' + i)});
                }
            }
        }
        
        std::vector<int> toDeletePidx;
        for(auto const& [pIdx, gIdx] : poolToGraphMap) {
            if (currentActive.find(pIdx) == currentActive.end()) {
                toDeletePidx.push_back(pIdx);
            }
        }
        
        if (!toDeletePidx.empty()) {
            std::vector<std::pair<int, int>> deleteList;
            for(int pIdx : toDeletePidx) deleteList.push_back({poolToGraphMap[pIdx], pIdx});
            
            std::sort(deleteList.rbegin(), deleteList.rend());
            
            for(auto const& [gIdxDeleted, pIdx] : deleteList) {
                graph.removeNodeAt(gIdxDeleted);
                poolToGraphMap.erase(pIdx);

                for(auto& [p, g] : poolToGraphMap) {
                    if (g > gIdxDeleted) g--;
                }
            }
        }
        
        graph.clearEdges();
        for(int pIdx : currentActive) {
            int srcGidx = poolToGraphMap[pIdx];
            for(int i=0; i<26; ++i) {
                int childPidx = pool[pIdx].children[i];
                if (childPidx != -1) {
                    int destGidx = poolToGraphMap[childPidx];
                    graph.addEdge(srcGidx, destGidx, "");
                }
            }
        }
    }

    void TrieController::triggerLayout(float duration) {
        auto layoutAnim = UI::DSA::LayoutEngine::asTrie(graph, model, poolToGraphMap, startX, startY, horizontalSpacing, verticalSpacing, duration);
        ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void TrieController::forceSnapLayout() {
        syncGraph(); 

        std::unordered_map<int, sf::Vector2f> targetPositions;
        const auto& pool = model.getPool();
        int rootIdx = model.getRootIndex();
        float currentLeafX = 0.f; 
        
        auto calculateDFS = [&](auto& self, int poolIdx, int depth) -> float {
            std::vector<int> children;
            for (int i = 0; i < 26; ++i) {
                if (pool[poolIdx].children[i] != -1) children.push_back(pool[poolIdx].children[i]);
            }
            
            float myX = 0.f;
            if (children.empty()) { 
                myX = currentLeafX;
                currentLeafX += horizontalSpacing;
            } else { 
                float sumX = 0;
                for (int child : children) sumX += self(self, child, depth + 1);
                myX = sumX / children.size();
            }
            targetPositions[poolIdx] = {myX, startY + depth * verticalSpacing};
            return myX;
        };
        
        if (rootIdx != -1) {
            calculateDFS(calculateDFS, rootIdx, 0);
            
            float offsetX = startX - targetPositions[rootIdx].x;
            
            for (auto& pair : targetPositions) {
                pair.second.x += offsetX;
                auto it = poolToGraphMap.find(pair.first);
                if (it != poolToGraphMap.end()) {
                    if (auto* node = graph.getNode(it->second)) {
                        node->setPosition(pair.second); 
                    }
                }
            }
        }
    }

    // ==================== CREATE RANDOM ====================
    void TrieController::handleCreateRandom(int count) {
        if (codeViewer) codeViewer->hide();

        // 1. Dọn dẹp sạch sẽ Trie cũ
        handleClearAll();

        // 2. Tạo ngẫu nhiên `count` từ (độ dài từ 3-5 ký tự)
        std::vector<std::string> randomWords;
        for (int i = 0; i < count; ++i) {
            int len = 3 + rand() % 3; // Độ dài 3, 4, hoặc 5
            std::string word = "";
            for (int j = 0; j < len; ++j) {
                word += (char)('a' + rand() % 26);
            }
            // Tránh trùng lặp từ ngẫu nhiên
            if (std::find(randomWords.begin(), randomWords.end(), word) == randomWords.end()) {
                randomWords.push_back(word);
                model.insert(word); // Insert ngầm vào Pool
            }
        }

        // 3. Ép UI vẽ và tản ra
        syncGraph();
        triggerLayout(0.6f);
    }

    // ==================== EDIT DATA FILE ====================
    void TrieController::handleEditDataFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/TrieData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::string header = "# --- TRIE VISUALIZER DATA ---\n"
                             "# DETAILED INSTRUCTIONS:\n"
                             "# 1. Type the number of words 'n' first.\n"
                             "# 2. Then type the 'n' words separated by spaces or newlines.\n"
                             "#    (Max n is 15. Words must contain ONLY English letters a-z).\n"
                             "# 3. Do NOT use numbers, commas (,) or other punctuation marks.\n"
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
                if (startPos == std::string::npos) {
                    if (!userContent.empty()) userContent += "\n";
                } else if (line[startPos] != '#') {
                    userContent += line + "\n";
                }
            }
            inFile.close();
        }

        std::ofstream outFile(filePath);
        if (outFile.is_open()) {
            outFile << header << userContent;
            outFile.close();
        }
        
        std::system(("start notepad " + filePath).c_str());
    }

    // ==================== CREATE FROM FILE ====================
    void TrieController::handleCreateFromFile() {
        if (codeViewer) codeViewer->hide();

        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/TrieData.txt";

        if (!std::filesystem::exists(dirPath)) {
            handleEditDataFile(); // Tự động tạo file và mở nếu chưa tồn tại
            std::cout << "[UI LOG] File not found. Created an empty file and opened Notepad.\n";
            return;
        }

        // Smart Parser
        std::string line;
        std::vector<std::string> allTokens;
        std::ifstream file(filePath);

        while (std::getline(file, line)) {
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos != std::string::npos && line[startPos] == '#') {
                continue; // Bỏ qua comment
            }

            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                allTokens.push_back(token);
            }
        }
        file.close();

        std::string errorMsg = "";
        int n = -1;
        std::vector<std::string> parsedData;

        if (allTokens.empty()) {
            errorMsg = "# [WARNING] Could not read 'n'. Please enter the number of words first.\n";
        } else {
            try {
                n = std::stoi(allTokens[0]);
                if (n < 0) {
                    errorMsg = "# [WARNING] Invalid count 'n' = " + std::to_string(n) + " (must be >= 0).\n";
                } else if (n > 15) {
                    errorMsg = "# [WARNING] Count 'n' = " + std::to_string(n) + " is too large. Maximum allowed is 15.\n";
                } else if (allTokens.size() - 1 < static_cast<size_t>(n)) {
                    errorMsg = "# [WARNING] Expected " + std::to_string(n) + " words, but found only " + std::to_string(allTokens.size() - 1) + ".\n";
                } else {
                    for (int i = 1; i <= n; ++i) {
                        std::string word = allTokens[i];
                        bool valid = true;
                        
                        // Kiểm tra và tự động convert in hoa -> in thường
                        for (char& c : word) {
                            if (c >= 'A' && c <= 'Z') c = c - 'A' + 'a';
                            if (c < 'a' || c > 'z') { valid = false; break; }
                        }
                        
                        if (!valid) {
                            errorMsg = "# [WARNING] Word '" + allTokens[i] + "' is invalid. Only English letters (a-z) are allowed.\n";
                            break;
                        }
                        parsedData.push_back(word);
                    }
                }
            } catch (...) {
                errorMsg = "# [WARNING] The first value must be an Integer indicating the number of words.\n";
            }
        }

        // Báo lỗi và ghi ngược lại vào Notepad
        if (!errorMsg.empty()) {
            std::cout << "[UI LOG] Data error. Opening Notepad to fix.\n";
            std::ifstream inFileForErr(filePath);
            std::string contentWithWarning = "";
            bool warningInserted = false;

            if (inFileForErr.is_open()) {
                std::string l;
                while (std::getline(inFileForErr, l)) {
                    if (l.find("# [WARNING]") != std::string::npos) continue;
                    contentWithWarning += l + "\n";
                    if (!warningInserted && l.find("# -----------------------------------") != std::string::npos) {
                        contentWithWarning += errorMsg;
                        warningInserted = true;
                    }
                }
                inFileForErr.close();
            }

            if (!warningInserted) contentWithWarning = errorMsg + contentWithWarning;

            std::ofstream outFileErr(filePath);
            if (outFileErr.is_open()) {
                outFileErr << contentWithWarning;
                outFileErr.close();
            }

            std::system(("start notepad " + filePath).c_str());
            return;
        }

        // Apply Data hoàn hảo
        handleClearAll(); // Xóa cũ đi

        for (const std::string& word : parsedData) {
            model.insert(word); // Nạp ngầm vào RAM
        }

        syncGraph();          // Ép UI vẽ lên màn hình
        triggerLayout(0.6f);  // Bay vào vị trí
    }

    // ==================== INSERT ====================
    void TrieController::handleInsert(const std::string& word) {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::insert();
        Builder b(codeDef, codeViewer);

        int tempCurr = model.getRootIndex();
        int existingChars = 0;
        for (char c : word) {
            int nextIdx = model.getPool()[tempCurr].children[c - 'a'];
            if (nextIdx != -1) {
                existingChars++;
                tempCurr = nextIdx;
            } else {
                break;
            }
        }

        model.insert(word); 

        b.highlight("init_curr");
        int currPoolIdx = model.getRootIndex();
        
        for (int i = 0; i < word.length(); ++i) {
            char c = word[i];
            int charIndex = c - 'a';
            int nextPoolIdx = model.getPool()[currPoolIdx].children[charIndex];

            b.highlight("loop_char");
            
            b.callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setFillColor(Config::UI::Colors::NodeHighlight); 
                        n->setLabelColor(Config::UI::Colors::LabelHighlight);
                    }
                }
            }).wait(0.2f);
            
            b.highlight("check_null");
            
            if (i >= existingChars) {
                b.highlight("create_node")
                 .callback([this, currPoolIdx, nextPoolIdx, c]() {
                     if (!poolToGraphMap.count(currPoolIdx)) return; 
                     int parentUiIdx = poolToGraphMap[currPoolIdx];
                     auto* parentNode = graph.getNode(parentUiIdx);
                     
                     if (!parentNode) return; // KHIÊN BẢO VỆ CHỐNG CRASH TỌA ĐỘ
                     sf::Vector2f parentPos = parentNode->getPosition();
                     
                     graph.addNode(std::string(1, c), parentPos); 
                     int newUiIdx = graph.getNodes().size() - 1;
                     poolToGraphMap[nextPoolIdx] = newUiIdx;
                     
                     graph.addEdge(parentUiIdx, newUiIdx, "");
                     triggerLayout();
                 })
                 .wait(0.4f);
            }
            
            b.highlight("advance")
             .callback([this, currPoolIdx]() {
                if (poolToGraphMap.count(currPoolIdx)) {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText);
                        if (model.getPool()[currPoolIdx].isEndOfWord) {
                            n->setFillColor(sf::Color(70, 160, 100)); 
                        } else {
                            n->setFillColor(Config::UI::Colors::NodeFill); 
                        }
                    }
                }
             }).wait(0.1f);
             
            currPoolIdx = nextPoolIdx;
        }

        b.highlight("set_end")
         .callback([this, currPoolIdx]() {
             if (poolToGraphMap.count(currPoolIdx)) {
                 if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                     n->setFillColor(sf::Color(70, 160, 100)); 
                 }
             }
             triggerLayout(0.2f);
         })
         .finish();

        ctx.animManager.addAnimation(b.build());
    }
    // ==================== SEARCH ====================
    void TrieController::handleSearch(const std::string& word, bool isPrefix) {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::search();
        Builder b(codeDef, codeViewer);

        b.highlight("init_curr");

        int currPoolIdx = model.getRootIndex();
        bool broken = false;

        for (char c : word) {
            b.highlight("loop_char");
            
            int charIndex = c - 'a';
            int nextPoolIdx = model.getPool()[currPoolIdx].children[charIndex];
            
            int uiIdx = poolToGraphMap[currPoolIdx];
            auto* uiNode = graph.getNode(uiIdx);
            
            b.nodeHighlight(uiNode, 0.2f);

            b.highlight("check_null");
            if (nextPoolIdx == -1) {
                b.highlight("not_found");
                b.callback([this, currPoolIdx]() {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText); 
                        if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                        else n->setFillColor(Config::UI::Colors::NodeFill);
                    }
                }).wait(0.1f);
                
                broken = true;
                break;
            }

            b.highlight("advance")
             .callback([this, currPoolIdx]() {
                 if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                     n->setLabelColor(Config::UI::Colors::NodeText); 
                     if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                     else n->setFillColor(Config::UI::Colors::NodeFill);
                 }
             }).wait(0.1f);

            currPoolIdx = nextPoolIdx; 
        }

        if (!broken) {
            b.highlight("check_end");
            
            int finalUiIdx = poolToGraphMap[currPoolIdx];
            auto* finalNode = graph.getNode(finalUiIdx);

            if (!model.getPool()[currPoolIdx].isEndOfWord){
                b.nodeHighlight(finalNode, 0.2f);
            }

            if (model.getPool()[currPoolIdx].isEndOfWord || isPrefix) {
                b.highlight("found")
                 .nodeScale(finalNode, 1.0f, 1.3f, 0.2f)
                 .nodeScale(finalNode, 1.3f, 1.0f, 0.2f)
                 .callback([this, currPoolIdx]() {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText);
                        if (!model.getPool()[currPoolIdx].isEndOfWord){
                            n->setFillColor(Config::UI::Colors::NodeFill);
                        }
                    }
                 });
            } else {
                b.highlight("not_found_end")
                 .callback([this, currPoolIdx]() {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText);
                        n->setFillColor(Config::UI::Colors::NodeFill);
                    }
                 });
            }
        }

        b.finish();
        ctx.animManager.addAnimation(b.build());
    }

    // ==================== REMOVE ====================
    void TrieController::handleRemove(const std::string& word) {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Trie::deleteWord();
        Builder b(codeDef, codeViewer);

        b.highlight("init_curr"); 
        int currPoolIdx = model.getRootIndex();
        bool foundPath = true;
        
        for (char c : word) {
            b.highlight("loop_char"); 
            
            int charIndex = c - 'a';
            int nextPoolIdx = model.getPool()[currPoolIdx].children[charIndex];
            
            int uiIdx = poolToGraphMap[currPoolIdx];
            auto* uiNode = graph.getNode(uiIdx);
            
            b.nodeHighlight(uiNode, 0.2f); 
            
            b.highlight("check_null"); 
            if (nextPoolIdx == -1) {
                b.highlight("not_found"); 
                
                b.callback([this, currPoolIdx]() {
                    if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                        n->setLabelColor(Config::UI::Colors::NodeText); 
                        if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                        else n->setFillColor(Config::UI::Colors::NodeFill);
                    }
                }).wait(0.1f);
                
                foundPath = false;
                break;
            }
            
            b.highlight("advance"); 
            b.callback([this, currPoolIdx]() {
                if (auto* n = graph.getNode(poolToGraphMap[currPoolIdx])) {
                    n->setLabelColor(Config::UI::Colors::NodeText); 
                    if (model.getPool()[currPoolIdx].isEndOfWord) n->setFillColor(sf::Color(70, 160, 100));
                    else n->setFillColor(Config::UI::Colors::NodeFill);
                }
            }).wait(0.1f);
            
            currPoolIdx = nextPoolIdx;
        }

        if (foundPath && currPoolIdx != -1) {
            b.highlight("unmark_end"); 
            
            int finalUiIdx = poolToGraphMap[currPoolIdx];
            b.nodeHighlight(graph.getNode(finalUiIdx), 0.2f);
            b.nodeUnhighlight(graph.getNode(finalUiIdx), 0.2f);
        }

        b.highlight("delete") 
         .callback([this, word]() {
             bool existed = model.search(word); 
             
             model.deleteWord(word); 
             
             if (existed) { 
                 syncGraph();
                 triggerLayout();
             } else {
                 std::cout << "[UI LOG] Không thể xóa từ: " << word << " (Không tồn tại)\n";
             }
         })
         .finish();

        ctx.animManager.addAnimation(b.build());
    }
    
    // ==================== CLEAR ====================
    void TrieController::handleClearAll() {
        if (codeViewer) codeViewer->hide();

        model.clear();
        graph.clear(); 
        poolToGraphMap.clear();

        graph.addNode("Root", {startX, startY});
        poolToGraphMap[model.getRootIndex()] = 0; 
        
        triggerLayout(0.f); 
    }

} // namespace Controllers