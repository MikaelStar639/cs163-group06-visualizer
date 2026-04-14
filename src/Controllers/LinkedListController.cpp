#include "Controllers/LinkedListController.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <sstream>

namespace Controllers {

    LinkedListController::LinkedListController(AppContext& context, UI::DSA::Graph& g, Core::DSA::LinkedList& m,
                                               UI::Widgets::PseudoCodeViewer* viewer)
        : ctx(context), graph(g), model(m), codeViewer(viewer) {}

    void LinkedListController::syncGraphEdges() {
        graph.clearEdges();
        int numNodes = graph.getNodes().size();
        for (int i = 0; i < numNodes - 1; ++i) {
            graph.addEdge(i, i + 1);
        }
    }

    void LinkedListController::triggerLayout(float duration) {
        auto layoutAnim = UI::DSA::LayoutEngine::asLinkedList(graph, startX, startY, spacing, duration);
        ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void LinkedListController::handleCreateRandom(int size) {
        if (codeViewer) codeViewer->hide();
        model.clear();
        graph.clear();

        for (int i = 0; i < size; ++i) {
            int randomVal = std::rand() % 100;
            model.insertTail(randomVal);
            graph.addNode(std::to_string(randomVal), {startX, startY});
        }
        syncGraphEdges();
        triggerLayout(0.6f);
    }

    void LinkedListController::handleCreateFromFile() {
        if (codeViewer) codeViewer->hide();

        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/LinkedListData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            // File does not exist, initialize it
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << "# --- LINKED LIST VISUALIZER DATA ---\n"
                        << "# DETAILED INSTRUCTIONS:\n"
                        << "# 1. Type the number of elements 'n' first.\n"
                        << "# 2. Then type the 'n' integer values separated by spaces or newlines.\n"
                        << "#    (Max n is 15. Values must be between -999 and 999).\n"
                        << "# 3. Do NOT use commas (,) or other punctuation marks.\n"
                        << "# 4. When you are done:\n"
                        << "#    - Save this file by pressing Ctrl + S\n"
                        << "#    - Go back to the Application and click the 'Go' button.\n"
                        << "# -----------------------------------\n";
                outFile.close();
            }
            std::cout << "[UI LOG] File not found. Created an empty file and opened Notepad for data entry.\n";
            std::system(("start notepad " + filePath).c_str());
            return;
        }

        // Smart Parser
        std::string line;
        std::vector<int> allNumbers;

        while (std::getline(file, line)) {
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos != std::string::npos && line[startPos] == '#') {
                continue; // Ignore comment/instruction lines
            }

            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                try {
                    int val = std::stoi(token);
                    allNumbers.push_back(val);
                } catch (...) {
                    // Ignore valid text
                }
            }
        }
        file.close();

        std::string errorMsg = "";
        int n = -1;
        std::vector<int> parsedData;

        if (allNumbers.empty()) {
            errorMsg = "# [WARNING] Could not read 'n'. Please enter the number of elements first.\n";
        } else {
            n = allNumbers[0];
            if (n < 0) {
                errorMsg = "# [WARNING] Invalid size 'n' = " + std::to_string(n) + " (must be >= 0).\n";
            } else if (n > 15) {
                errorMsg = "# [WARNING] Size 'n' = " + std::to_string(n) + " is too large. Maximum allowed is 15.\n";
            } else if (allNumbers.size() - 1 < static_cast<size_t>(n)) {
                errorMsg = "# [WARNING] Expected " + std::to_string(n) + " elements, but found only " + std::to_string(allNumbers.size() - 1) + ".\n";
            } else {
                for (int i = 1; i <= n; ++i) {
                    if (allNumbers[i] < -999 || allNumbers[i] > 999) {
                        errorMsg = "# [WARNING] Value " + std::to_string(allNumbers[i]) + " has more than 3 digits (must be between -999 and 999).\n";
                        break;
                    }
                    parsedData.push_back(allNumbers[i]);
                }
            }
        }

        if (!errorMsg.empty()) {
            std::cout << "[UI LOG] Data error. Opening Notepad to fix.\n";
            
            // Read file again to preserve content but inject warning
            std::ifstream inFileForErr(filePath);
            std::string contentWithWarning = "";
            bool warningInserted = false;

            if (inFileForErr.is_open()) {
                std::string l;
                while (std::getline(inFileForErr, l)) {
                    // Prevent piling up old warnings
                    if (l.find("# [WARNING]") != std::string::npos) continue;
                    
                    contentWithWarning += l + "\n";

                    if (!warningInserted && l.find("# -----------------------------------") != std::string::npos) {
                        contentWithWarning += errorMsg;
                        warningInserted = true;
                    }
                }
                inFileForErr.close();
            }

            if (!warningInserted) {
                contentWithWarning = errorMsg + contentWithWarning;
            }

            std::ofstream outFileErr(filePath);
            if (outFileErr.is_open()) {
                outFileErr << contentWithWarning;
                outFileErr.close();
            }

            std::system(("start notepad " + filePath).c_str());
            return;
        }

        model.clear();
        graph.clear();

        for (int val : parsedData) {
            model.insertTail(val);
            graph.addNode(std::to_string(val), {startX, startY});
        }

        syncGraphEdges();
        triggerLayout(0.6f);
    }

    void LinkedListController::handleEditDataFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/LinkedListData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::string header = "# --- LINKED LIST VISUALIZER DATA ---\n"
                             "# DETAILED INSTRUCTIONS:\n"
                             "# 1. Type the number of elements 'n' first.\n"
                             "# 2. Then type the 'n' integer values separated by spaces or newlines.\n"
                             "#    (Max n is 15. Values must be between -999 and 999).\n"
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
                if (startPos == std::string::npos) {
                    // Avoid writing too many empty lines. Only keep empty lines if userContent is not empty.
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

    // ==================== INSERT ====================

    void LinkedListController::handleInsert(int sel, int pos, int val) {
        using Builder = UI::Animations::AnimStepBuilder;

        if (sel == 0) { // HEAD
            auto codeDef = Core::DSA::PseudoCode::LinkedList::insertHead();
            Builder b(codeDef, codeViewer);

            b.highlight("create_node").wait()
             .highlight("link_next").wait()
             .highlight("update_head")
             .callback([this, val]() {
                 model.insertHead(val);
                 graph.insertNodeAt(0, std::to_string(val), {startX - spacing, startY});
                 syncGraphEdges();
                 triggerLayout();
             })
             .finish();

            ctx.animManager.addAnimation(b.build());
            return;
        }

        // TAIL & AT POS
        int currentSize = (int)graph.getNodes().size();
        std::cout << "[UI DEBUG] handleInsert: sel=" << sel << ", pos=" << pos << ", size=" << currentSize << std::endl;
        
        // Bounds check for AT POS
        if (sel == 2 && (pos < 0 || pos > currentSize)) {
            std::cout << "[UI LOG] Invalid position: " << pos << " (size: " << currentSize << ")" << std::endl;
            auto codeDef = Core::DSA::PseudoCode::LinkedList::insertAt();
            Builder b(codeDef, codeViewer);
            b.highlight("bounds_check").finish();
            ctx.animManager.addAnimation(b.build());
            return;
        }

        int targetPos = (sel == 1) ? currentSize : pos;

        if (sel == 1) {
            // INSERT TAIL
            auto codeDef = Core::DSA::PseudoCode::LinkedList::insertTail();
            Builder b(codeDef, codeViewer);

            b.highlight("check_null");
            if (graph.getNodes().empty()) {
                b.highlight("insert_empty");
            } else {
                b.highlight("else")
                 .highlight("init_curr");
            }

            // TRAVERSAL
            int steps = (int)graph.getNodes().size() - 1;
            for (int i = 0; i < steps; ++i) {
                auto* uiNode = graph.getNode(i);
                if (uiNode) {
                    b.highlight("loop_cond")
                     .nodeHighlight(uiNode, 0.2f)
                     .highlight("advance")
                     .nodeUnhighlight(uiNode, 0.1f);
                }
            }

            b.highlight("insert_tail")
             .callback([this, val]() {
                 model.insertTail(val);
                 int actualPos = model.getLogicalList().size() - 1;
                 graph.insertNodeAt(actualPos, std::to_string(val), {startX + actualPos * spacing, startY - 100.f});
                 syncGraphEdges();
                 triggerLayout();
             })
             .finish();

            ctx.animManager.addAnimation(b.build());

        } else {
            // INSERT AT POS
            auto codeDef = Core::DSA::PseudoCode::LinkedList::insertAt();
            Builder b(codeDef, codeViewer);

            b.highlight("bounds_check")
             .highlight("check_head")
             .highlight("else")
             .highlight("init_pre");

            // TRAVERSAL
            int steps = targetPos - 1;
            for (int i = 0; i < steps; ++i) {
                auto* uiNode = graph.getNode(i);
                if (uiNode) {
                    b.highlight("loop_cond")
                     .nodeHighlight(uiNode, 0.2f)
                     .highlight("advance")
                     .nodeUnhighlight(uiNode, 0.1f);
                }
            }

            b.highlight("create_node")
             .callback([this, targetPos, val]() {
                 bool success = model.insertAt(targetPos, val);
                 if (success) {
                     graph.insertNodeAt(targetPos, std::to_string(val), {startX + targetPos * spacing, startY - 100.f});
                     syncGraphEdges();
                     triggerLayout();
                 }
             })
             .highlight("link_next").wait(0.3f)
             .highlight("link_pre")
             .finish();

            ctx.animManager.addAnimation(b.build());
        }
    }

    // ==================== REMOVE ====================

    void LinkedListController::handleRemove(int sel, int pos) {
        using Builder = UI::Animations::AnimStepBuilder;

        if (sel == 0) { // HEAD
            auto codeDef = Core::DSA::PseudoCode::LinkedList::deleteHead();
            Builder b(codeDef, codeViewer);

            b.highlight("check_null")
             .highlight("save_temp").wait()
             .highlight("advance").wait()
             .highlight("delete")
             .callback([this]() {
                 model.deleteHead();
                 graph.removeNodeAt(0);
                 syncGraphEdges();
                 triggerLayout();
             })
             .finish();

            ctx.animManager.addAnimation(b.build());
            return;
        }

        // TAIL & AT POS
        int currentSize = (int)graph.getNodes().size();
        int targetPos = (sel == 1) ? currentSize - 1 : pos;

        std::cout << "[UI DEBUG] handleRemove: sel=" << sel << ", targetPos=" << targetPos << ", size=" << currentSize << std::endl;

        if (targetPos < 0 || targetPos >= currentSize) {
            std::cout << "[UI LOG] Invalid position for removal: " << targetPos << std::endl;
            if (sel == 1) {
                auto codeDef = Core::DSA::PseudoCode::LinkedList::deleteTail();
                Builder b(codeDef, codeViewer);
                b.highlight("check_null").finish();
                ctx.animManager.addAnimation(b.build());
            } else {
                auto codeDef = Core::DSA::PseudoCode::LinkedList::deleteAt();
                Builder b(codeDef, codeViewer);
                b.highlight("bounds_check").finish();
                ctx.animManager.addAnimation(b.build());
            }
            return;
        }

        if (sel == 1) {
            // DELETE TAIL
            auto codeDef = Core::DSA::PseudoCode::LinkedList::deleteTail();
            Builder b(codeDef, codeViewer);

            b.highlight("check_null")
             .highlight("single_node");
            if (currentSize > 1) {
                b.highlight("else")
                 .highlight("init_pre");
            }

            // TRAVERSAL
            int steps = currentSize - 2;
            for (int i = 0; i < steps; ++i) {
                auto* uiNode = graph.getNode(i);
                if (uiNode) {
                    b.highlight("loop_cond")
                     .nodeHighlight(uiNode, 0.2f)
                     .highlight("advance")
                     .nodeUnhighlight(uiNode, 0.1f);
                }
            }

            b.highlight("save_del")
             .callback([this]() {
                 model.deleteTail();
                 graph.removeNodeAt((int)graph.getNodes().size() - 1);
                 syncGraphEdges();
                 triggerLayout();
             })
             .highlight("unlink").wait(0.3f)
             .highlight("delete")
             .finish();

            ctx.animManager.addAnimation(b.build());

        } else {
            // DELETE AT POS
            auto codeDef = Core::DSA::PseudoCode::LinkedList::deleteAt();
            Builder b(codeDef, codeViewer);

            b.highlight("bounds_check")
             .highlight("check_head");
            if (targetPos > 0) {
                b.highlight("else")
                 .highlight("init_pre");
            }

            // TRAVERSAL
            int steps = targetPos - 1;
            for (int i = 0; i < steps; ++i) {
                auto* uiNode = graph.getNode(i);
                if (uiNode) {
                    b.highlight("loop_cond")
                     .nodeHighlight(uiNode, 0.2f)
                     .highlight("advance")
                     .nodeUnhighlight(uiNode, 0.1f);
                }
            }

            b.highlight("save_del")
             .callback([this, targetPos]() {
                 model.deleteAt(targetPos);
                 graph.removeNodeAt(targetPos);
                 syncGraphEdges();
                 triggerLayout();
             })
             .highlight("unlink").wait(0.3f)
             .highlight("delete")
             .finish();

            ctx.animManager.addAnimation(b.build());
        }
    }

    // ==================== SEARCH ====================

    void LinkedListController::handleSearch(int targetValue) {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::LinkedList::search();
        Builder b(codeDef, codeViewer);

        b.highlight("init_curr");

        int curr = model.getHeadIndex();
        int idx = 0;
        bool found = false;

        while (curr != -1) {
            UI::DSA::Node* uiNode = graph.getNode(idx);
            if (!uiNode) break;

            b.highlight("loop_cond")
             .nodeHighlight(uiNode, 0.3f)
             .highlight("check_val");

            if (model.getPool()[curr].value == targetValue) {
                b.highlight("found")
                 .nodeScale(uiNode, 1.0f, 1.3f, 0.2f)
                 .nodeScale(uiNode, 1.3f, 1.0f, 0.2f)
                 .nodeUnhighlight(uiNode, 0.3f);
                found = true;
                break; 
            } else {
                b.highlight("advance")
                 .nodeUnhighlight(uiNode, 0.1f);
            }
            curr = model.getPool()[curr].nextIndex;
            idx++;
        }

        if (!found) {
            b.highlight("not_found");
            std::cout << "[UI LOG] Value not found: " << targetValue << std::endl;
        }

        b.finish();
        ctx.animManager.addAnimation(b.build());
    }

    // ==================== UPDATE ====================

    void LinkedListController::handleUpdate(int sel, int pos, int oldVal, int newVal) {
        using Builder = UI::Animations::AnimStepBuilder;

        if (sel == 0) { // UPDATE AT POS
            int currentSize = (int)graph.getNodes().size();
            std::cout << "[UI DEBUG] handleUpdate: pos=" << pos << ", size=" << currentSize << std::endl;

            if (pos < 0 || pos >= currentSize) {
                std::cout << "[UI LOG] Invalid position for update: " << pos << std::endl;
                auto codeDef = Core::DSA::PseudoCode::LinkedList::updateAt();
                Builder b(codeDef, codeViewer);
                b.highlight("bounds_check").finish();
                ctx.animManager.addAnimation(b.build());
                return;
            }

            auto codeDef = Core::DSA::PseudoCode::LinkedList::updateAt();
            Builder b(codeDef, codeViewer);

            b.highlight("bounds_check")
             .highlight("init_curr");

            // TRAVERSAL
            for (int i = 0; i < pos; ++i) {
                auto* uiNode = graph.getNode(i);
                if (!uiNode) continue;
                b.highlight("loop_cond")
                 .nodeHighlight(uiNode, 0.2f)
                 .highlight("advance")
                 .nodeUnhighlight(uiNode, 0.1f);
            }

            // Target node
            auto* uiNode = graph.getNode(pos);
            if (uiNode) {
                b.highlight("loop_cond")
                 .nodeHighlight(uiNode, 0.2f)
                 .highlight("update_val")
                 .nodeScale(uiNode, 1.0f, 1.2f, 0.15f)
                 .callback([this, pos, newVal]() {
                     if (model.updateAt(pos, newVal)) {
                         graph.updateNodeValue(pos, std::to_string(newVal));
                     }
                 })
                 .nodeScale(uiNode, 1.2f, 1.0f, 0.15f)
                 .nodeUnhighlight(uiNode, 0.2f);
            }

            b.finish();
            ctx.animManager.addAnimation(b.build());

        } else if (sel == 1) { // UPDATE BY VALUE
            auto codeDef = Core::DSA::PseudoCode::LinkedList::updateByValue();
            Builder b(codeDef, codeViewer);

            b.highlight("init_curr");

            int curr = model.getHeadIndex();
            int idx = 0;
            bool found = false;

            while (curr != -1) {
                UI::DSA::Node* uiNode = graph.getNode(idx);
                if (!uiNode) break;

                b.highlight("loop_cond")
                 .nodeHighlight(uiNode, 0.2f)
                 .highlight("check_val");

                if (model.getPool()[curr].value == oldVal) {
                    b.highlight("update_val")
                     .nodeScale(uiNode, 1.0f, 1.2f, 0.15f)
                     .callback([this, oldVal, newVal, idx]() {
                         if (model.updateValue(oldVal, newVal)) {
                             graph.updateNodeValue(idx, std::to_string(newVal));
                         }
                     })
                     .highlight("found")
                     .nodeScale(uiNode, 1.2f, 1.0f, 0.15f)
                     .nodeUnhighlight(uiNode, 0.2f);
                    found = true;
                    break;
                } else {
                    b.highlight("advance")
                     .nodeUnhighlight(uiNode, 0.1f);
                }
                curr = model.getPool()[curr].nextIndex;
                idx++;
            }

            if (!found) {
                b.highlight("not_found");
            }

            b.finish();
            ctx.animManager.addAnimation(b.build());
        }
    }

    // ==================== CLEAR ====================

    void LinkedListController::handleClearAll() {
        if (codeViewer) codeViewer->hide();

        if (graph.isAnimating()) {
            model.clear();
            graph.clear(); 
            return;
        }

        graph.clearEdges();
        int currentSize = graph.getNodes().size();
        for (int i = 0; i < currentSize; ++i) {
            graph.removeNodeAt(0); 
        }
        model.clear();
    }

} // namespace Controllers