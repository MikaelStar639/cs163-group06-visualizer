#include "Controllers/LinkedListController.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include "UI/Animations/Core/WaitAnimation.hpp"
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

    void LinkedListController::addHighlight(std::unique_ptr<UI::Animations::SequenceAnimation>& seq, int line) {
        if (!codeViewer) return;
        seq->add(std::make_unique<UI::Animations::CallbackAnimation>([this, line]() {
            codeViewer->highlightLine(line);
        }));
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

    void LinkedListController::handleInsert(int sel, int pos, int val) {
        if (sel == 0) { // HEAD
            auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::insertHead());

            addHighlight(sequence, 0); // "newNode = new Node(val)"
            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.4f));
            addHighlight(sequence, 1); // "newNode.next = head"
            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.4f));
            addHighlight(sequence, 2); // "head = newNode"

            sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, val]() {
                model.insertHead(val);
                graph.insertNodeAt(0, std::to_string(val), {startX - spacing, startY});
                syncGraphEdges();
                triggerLayout();
            }));

            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.6f));
            sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
                if (codeViewer) codeViewer->hide();
            }));

            ctx.animManager.addAnimation(std::move(sequence));
            return;
        }

        // TAIL & AT POS
        int targetPos = (sel == 1) ? model.getLogicalList().size() : pos;
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        // Set pseudo-code based on operation
        if (sel == 1) {
            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::insertTail());
        } else {
            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::insertAt());
        }

        addHighlight(sequence, 0); // "curr = head"

        int steps = std::min(targetPos, (int)graph.getNodes().size());
        for (int i = 0; i < steps; ++i) {
            auto* uiNode = graph.getNode(i);
            if (uiNode) {
                addHighlight(sequence, 1); // "while/for loop"
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                addHighlight(sequence, 2); // "curr = curr.next"
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
            }
        }

        // Highlight the insert lines
        int insertLineStart = (sel == 1) ? 3 : 3; // both start at line 3
        addHighlight(sequence, insertLineStart); // "newNode = new Node(val)"

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, sel, targetPos, val, insertLineStart]() {
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

        addHighlight(sequence, insertLineStart + 1); // "curr.next = newNode" or "newNode.next = curr.next"
        sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.5f));

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
            if (codeViewer) codeViewer->hide();
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleRemove(int sel, int pos) {
        if (sel == 0) { // HEAD
            auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::deleteHead());

            addHighlight(sequence, 0); // "temp = head"
            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.4f));
            addHighlight(sequence, 1); // "head = head.next"
            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.4f));
            addHighlight(sequence, 2); // "delete temp"

            sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
                model.deleteHead();
                graph.removeNodeAt(0);
                syncGraphEdges();
                triggerLayout();
            }));

            sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.6f));
            sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
                if (codeViewer) codeViewer->hide();
            }));

            ctx.animManager.addAnimation(std::move(sequence));
            return;
        }

        // TAIL & AT POS
        int currentSize = model.getLogicalList().size();
        if (currentSize == 0) return;
        
        int targetPos = (sel == 1) ? currentSize - 1 : pos;
        if (targetPos < 0 || targetPos >= currentSize) return;

        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        // Set pseudo-code based on operation
        if (sel == 1) {
            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::deleteTail());
        } else {
            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::deleteAt());
        }

        addHighlight(sequence, 0); // "curr = head"

        for (int i = 0; i <= targetPos; ++i) {
            auto* uiNode = graph.getNode(i);
            if (uiNode) {
                addHighlight(sequence, 1); // "while/for loop"
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                if (i < targetPos) {
                    addHighlight(sequence, 2); // "curr = curr.next"
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
            }
        }

        // Highlight the delete lines
        int deleteLineStart = 3; // "temp = curr.next" or "delete curr.next"
        addHighlight(sequence, deleteLineStart);

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, sel, targetPos]() {
            if (sel == 1) model.deleteTail();
            else if (sel == 2) model.deleteAt(targetPos);
            
            graph.removeNodeAt(targetPos);
            syncGraphEdges();
            triggerLayout();
        }));

        addHighlight(sequence, deleteLineStart + 1);
        sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.5f));

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
            if (codeViewer) codeViewer->hide();
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleSearch(int targetValue) {
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::search());

        addHighlight(sequence, 0); // "curr = head"

        int curr = model.getHeadIndex();
        int idx = 0;
        bool found = false;

        while (curr != -1) {
            UI::DSA::Node* uiNode = graph.getNode(idx);
            if (!uiNode) break;

            addHighlight(sequence, 1); // "while curr != null:"
            sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.3f));

            addHighlight(sequence, 2); // "if curr.val == target:"

            if (model.getPool()[curr].value == targetValue) {
                addHighlight(sequence, 3); // "return FOUND"
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.3f, 0.2f));
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.3f, 1.0f, 0.2f)); 
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.3f));
                found = true;
                break; 
            } else {
                addHighlight(sequence, 4); // "curr = curr.next"
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
            }
            curr = model.getPool()[curr].nextIndex;
            idx++;
        }

        if (!found) {
            addHighlight(sequence, 5); // "return NOT FOUND"
            std::cout << "[UI LOG] Value not found: " << targetValue << std::endl;
        }

        sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.5f));
        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
            if (codeViewer) codeViewer->hide();
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void LinkedListController::handleUpdate(int sel, int pos, int oldVal, int newVal) {
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        if (sel == 0) { // UPDATE AT POS
            int currentSize = model.getLogicalList().size();
            if (pos < 0 || pos >= currentSize) return; 

            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::updateAt());

            addHighlight(sequence, 0); // "curr = head"

            for (int i = 0; i <= pos; ++i) {
                auto* uiNode = graph.getNode(i);
                if (!uiNode) continue;

                addHighlight(sequence, 1); // "for i = 0 to pos:"
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));
                
                if (i == pos) {
                    addHighlight(sequence, 3); // "curr.val = newVal"
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.2f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, pos, newVal]() {
                        if (model.updateAt(pos, newVal)) {
                            graph.updateNodeValue(pos, std::to_string(newVal));
                        }
                    }));
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.2f, 1.0f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.2f));
                } else {
                    addHighlight(sequence, 2); // "curr = curr.next"
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
            }

        } else if (sel == 1) { // UPDATE BY VALUE
            if (codeViewer) codeViewer->setCode(Core::DSA::PseudoCode::updateByValue());

            addHighlight(sequence, 0); // "curr = head"

            int curr = model.getHeadIndex();
            int idx = 0;
            bool found = false;

            while (curr != -1) {
                UI::DSA::Node* uiNode = graph.getNode(idx);
                if (!uiNode) break;

                addHighlight(sequence, 1); // "while curr != null:"
                sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.2f));

                addHighlight(sequence, 2); // "if curr.val == oldVal:"

                if (model.getPool()[curr].value == oldVal) {
                    addHighlight(sequence, 3); // "curr.val = newVal"
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.2f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this, oldVal, newVal, idx]() {
                        if (model.updateValue(oldVal, newVal)) {
                            graph.updateNodeValue(idx, std::to_string(newVal));
                        }
                    }));
                    addHighlight(sequence, 4); // "return"
                    sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.2f, 1.0f, 0.15f));
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.2f));
                    found = true;
                    break;
                } else {
                    addHighlight(sequence, 5); // "curr = curr.next"
                    sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
                }
                curr = model.getPool()[curr].nextIndex;
                idx++;
            }

            if (!found) {
                std::cout << "[UI LOG] Value " << oldVal << " not found for update!\n";
            }
        }

        sequence->add(std::make_unique<UI::Animations::WaitAnimation>(0.5f));
        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>([this]() {
            if (codeViewer) codeViewer->hide();
        }));

        ctx.animManager.addAnimation(std::move(sequence));
    }

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