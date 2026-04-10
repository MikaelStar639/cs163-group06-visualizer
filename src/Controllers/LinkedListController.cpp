#include "Controllers/LinkedListController.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <sstream>

namespace Controllers {

    LinkedListController::LinkedListController(AppContext& context, UI::DSA::Graph& g, Core::DSA::LinkedList& m)
        : ctx(context), graph(g), model(m) {}

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

    void LinkedListController::handleInsert(int sel, int pos, int val) {
        if (sel == 0) { // HEAD
            model.insertHead(val);
            graph.insertNodeAt(0, std::to_string(val), {startX - spacing, startY});
            syncGraphEdges();
            triggerLayout();
            return;
        }

        // TAIL & AT POS
        int targetPos = (sel == 1) ? model.getLogicalList().size() : pos;
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        int steps = std::min(targetPos, (int)graph.getNodes().size());
        for (int i = 0; i < steps; ++i) {
            auto* uiNode = graph.getNode(i);
            if (uiNode) {
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
            }
        }

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, sel, targetPos, val]() {
            bool success = false;
            int actualPos = 0;

            if (sel == 1) { 
                model.insertTail(val);
                actualPos = model.getLogicalList().size() - 1;
                success = true;
            } else if (sel == 2) { 
                success = model.insertAt(targetPos, val);
                actualPos = targetPos;
            }

            if (success) {
                graph.insertNodeAt(actualPos, std::to_string(val), {startX + actualPos * spacing, startY - 100.f});
                syncGraphEdges();
                triggerLayout();
            }
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleRemove(int sel, int pos) {
        if (sel == 0) { // HEAD
            model.deleteHead();
            graph.removeNodeAt(0);
            syncGraphEdges();
            triggerLayout();
            return;
        }

        // TAIL & AT POS
        int currentSize = model.getLogicalList().size();
        if (currentSize == 0) return;
        
        int targetPos = (sel == 1) ? currentSize - 1 : pos;
        if (targetPos < 0 || targetPos >= currentSize) return;

        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        for (int i = 0; i <= targetPos; ++i) {
            auto* uiNode = graph.getNode(i);
            if (uiNode) {
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                if (i < targetPos) {
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
            }
        }

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, sel, targetPos]() {
            if (sel == 1) model.deleteTail();
            else if (sel == 2) model.deleteAt(targetPos);
            
            graph.removeNodeAt(targetPos);
            syncGraphEdges();
            triggerLayout();
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleSearch(int targetValue) {
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();
        int curr = model.getHeadIndex();
        int idx = 0;
        bool found = false;

        while (curr != -1) {
            UI::DSA::Node* uiNode = graph.getNode(idx);
            if (!uiNode) break;

            sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.3f));

            if (model.getPool()[curr].value == targetValue) {
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.3f, 0.2f));
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.3f, 1.0f, 0.2f)); 
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.3f));
                found = true;
                break; 
            } else {
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
            }
            curr = model.getPool()[curr].nextIndex;
            idx++;
        }

        if (!found) {
            std::cout << "[UI LOG] Value not found: " << targetValue << std::endl;
        }

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleUpdate(int sel, int pos, int oldVal, int newVal) {
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        if (sel == 0) { // UPDATE AT POS
            int currentSize = model.getLogicalList().size();
            if (pos < 0 || pos >= currentSize) return; 

            for (int i = 0; i <= pos; ++i) {
                auto* uiNode = graph.getNode(i);
                if (!uiNode) continue;

                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                
                if (i == pos) {
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.2f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, pos, newVal]() {
                        if (model.updateAt(pos, newVal)) {
                            graph.updateNodeValue(pos, std::to_string(newVal));
                        }
                    }));
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.2f, 1.0f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.2f));
                } else {
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
            }

        } else if (sel == 1) { // UPDATE BY VALUE
            int curr = model.getHeadIndex();
            int idx = 0;
            bool found = false;

            while (curr != -1) {
                UI::DSA::Node* uiNode = graph.getNode(idx);
                if (!uiNode) break;

                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));

                if (model.getPool()[curr].value == oldVal) {
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.2f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, oldVal, newVal, idx]() {
                        if (model.updateValue(oldVal, newVal)) {
                            graph.updateNodeValue(idx, std::to_string(newVal));
                        }
                    }));
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.2f, 1.0f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.2f));
                    found = true;
                    break;
                } else {
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
                curr = model.getPool()[curr].nextIndex;
                idx++;
            }

            if (!found) {
                std::cout << "[UI LOG] Value " << oldVal << " not found for update!\n";
            }
        }

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleClearAll() {
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