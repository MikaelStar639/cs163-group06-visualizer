#include "Controllers/HeapController.hpp"
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
#include <cmath>

namespace Controllers {
    namespace {
    constexpr float HEAP_COMPARE_HIGHLIGHT = 0.35f;
    constexpr float HEAP_COMPARE_UNHIGHLIGHT = 0.18f;
    constexpr float HEAP_NODE_SCALE_UP = 0.20f;
    constexpr float HEAP_NODE_SCALE_DOWN = 0.20f;
}

    HeapController::HeapController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Heap& m)
        : ctx(context), graph(g), model(m) {}

    void HeapController::syncGraphEdges() {
        graph.clearEdges();

        int n = static_cast<int>(graph.getNodes().size());
        for (int i = 0; i < n; ++i) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < n)  graph.addEdge(i, left);
            if (right < n) graph.addEdge(i, right);
        }
    }

    void HeapController::triggerLayout(float /*duration*/) {
        int n = static_cast<int>(graph.getNodes().size());
        if (n == 0) return;

        int maxLevel = static_cast<int>(std::floor(std::log2(n)));

        for (int i = 0; i < n; ++i) {
            int level = static_cast<int>(std::floor(std::log2(i + 1)));
            int firstIndexInLevel = (1 << level) - 1;
            int posInLevel = i - firstIndexInLevel;
            int nodesInLevel = 1 << level;

            float spacing = baseGap * std::pow(2.f, static_cast<float>(maxLevel - level));
            float firstX = centerX - spacing * (nodesInLevel - 1) / 2.f;
            float x = firstX + posInLevel * spacing;
            float y = startY + level * levelGap;

            auto* uiNode = graph.getNode(i);
            if (uiNode) {
                uiNode->setPosition({x, y});
            }
        }
    }

    void HeapController::rebuildGraphFromModel() {
        graph.clear();

        const auto& pool = model.getPool();
        for (int val : pool) {
            graph.addNode(std::to_string(val), {centerX, startY});
        }

        syncGraphEdges();
        triggerLayout(0.6f);
    }

    void HeapController::handleCreateRandom(int size) {
        if (size <= 0) return;

        model.clear();
        graph.clear();

        for (int i = 0; i < size; ++i) {
            int randomVal = std::rand() % 100;
            model.insert(randomVal);
        }

        rebuildGraphFromModel();
    }

    void HeapController::handleCreateFromFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/HeapData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << "# --- HEAP VISUALIZER DATA ---\n"
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

        std::string line;
        std::vector<int> allNumbers;

        while (std::getline(file, line)) {
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos != std::string::npos && line[startPos] == '#') {
                continue;
            }

            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                try {
                    int val = std::stoi(token);
                    allNumbers.push_back(val);
                } catch (...) {
                    // Ignore non-number token
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
                errorMsg = "# [WARNING] Expected " + std::to_string(n) + " elements, but found only "
                         + std::to_string(allNumbers.size() - 1) + ".\n";
            } else {
                for (int i = 1; i <= n; ++i) {
                    if (allNumbers[i] < -999 || allNumbers[i] > 999) {
                        errorMsg = "# [WARNING] Value " + std::to_string(allNumbers[i])
                                 + " has more than 3 digits (must be between -999 and 999).\n";
                        break;
                    }
                    parsedData.push_back(allNumbers[i]);
                }
            }
        }

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
            model.insert(val);
        }

        rebuildGraphFromModel();
    }

    void HeapController::handleEditDataFile() {
        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/HeapData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::string header = "# --- HEAP VISUALIZER DATA ---\n"
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

    void HeapController::handleInsert(int val) {
        std::vector<int> before = model.getPool();
        auto steps = buildInsertSteps(before, val);

        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        for (const auto& step : steps) {
            if (step.type == HeapStepType::InsertLastVisual) {
                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, val]() {
                        graph.addNode(std::to_string(val), {centerX, startY});
                        syncGraphEdges();
                        triggerLayout();
                    }
                ));
            }
            else if (step.type == HeapStepType::Compare) {
                auto* a = graph.getNode(step.i);
                auto* b = graph.getNode(step.j);

                if (a) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(a, HEAP_COMPARE_HIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(b, HEAP_COMPARE_HIGHLIGHT));
                if (a) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(a, HEAP_COMPARE_UNHIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(b, HEAP_COMPARE_UNHIGHLIGHT));
            }
            else if (step.type == HeapStepType::ApplyState) {
                auto state = step.state;
                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, state]() {
                        applyStateToGraph(state);
                    }
                ));
            }
        }

    sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
        [this, val]() {
            model.insert(val);
        }
    ));

    ctx.animManager.addAnimation(std::move(sequence));
}

    void HeapController::handleRemove(int val) {
        std::vector<int> before = model.getPool();
        auto steps = buildDeleteSteps(before, val);

        if (steps.empty()) {
            std::cout << "[UI LOG] Value not found: " << val << '\n';
            return;
        }

        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        for (const auto& step : steps) {
            if (step.type == HeapStepType::Compare) {
                auto* a = graph.getNode(step.i);
                auto* b = graph.getNode(step.j);

                if (a) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(a, HEAP_COMPARE_HIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(b, HEAP_COMPARE_HIGHLIGHT));
                if (a) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(a, HEAP_COMPARE_UNHIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(b, HEAP_COMPARE_UNHIGHLIGHT));
            }
            else if (step.type == HeapStepType::OverwriteValue) {
                int idx = step.i;
                int value = step.value;

                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, idx, value]() {
                        graph.setNodeValueRaw(idx, std::to_string(value));
                    }
                ));
            }
            else if (step.type == HeapStepType::RemoveLastVisual) {
                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this]() {
                        graph.removeLastNode();
                        syncGraphEdges();
                        triggerLayout();
                    }
                ));
            }
            else if (step.type == HeapStepType::ApplyState) {
                auto state = step.state;
                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, state]() {
                        applyStateToGraph(state);
                    }
                ));
            }
        }

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
            [this, val]() {
                model.deleteValue(val);
            }
        ));

        ctx.animManager.addAnimation(std::move(sequence));
    }
    void HeapController::handleSearch(int targetValue) {
        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();
        const auto& pool = model.getPool();
        bool found = false;

        for (int i = 0; i < static_cast<int>(pool.size()); ++i) {
            UI::DSA::Node* uiNode = graph.getNode(i);
            if (!uiNode) break;

            sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(uiNode, 0.3f));

            if (pool[i] == targetValue) {
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.0f, 1.3f, 0.2f));
                sequence->add(std::make_unique<UI::Animations::NodeScaleAnimation>(uiNode, 1.3f, 1.0f, 0.2f));
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.3f));
                found = true;
                break;
            } else {
                sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(uiNode, 0.1f));
            }
        }

        if (!found) {
            std::cout << "[UI LOG] Value not found: " << targetValue << std::endl;
        }

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void HeapController::handleUpdate(int oldVal, int newVal) {
        std::vector<int> before = model.getPool();
        auto steps = buildUpdateSteps(before, oldVal, newVal);

        if (steps.empty()) {
            std::cout << "[UI LOG] Value " << oldVal << " not found for update!\n";
            return;
        }

        auto sequence = std::make_unique<UI::Animations::SequenceAnimation>();

        for (const auto& step : steps) {
            if (step.type == HeapStepType::Compare) {
                auto* a = graph.getNode(step.i);
                auto* b = graph.getNode(step.j);

                if (a) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(a, HEAP_COMPARE_HIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(b, HEAP_COMPARE_HIGHLIGHT));
                if (a) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(a, HEAP_COMPARE_UNHIGHLIGHT));
                if (b) sequence->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(b, HEAP_COMPARE_UNHIGHLIGHT));
            }
            else if (step.type == HeapStepType::OverwriteValue) {
                int idx = step.i;
                int value = step.value;

                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, idx, value]() {
                        graph.setNodeValueRaw(idx, std::to_string(value));
                    }
                ));
            }
            else if (step.type == HeapStepType::ApplyState) {
                auto state = step.state;
                sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
                    [this, state]() {
                        applyStateToGraph(state);
                    }
                ));
            }
        }

        sequence->add(std::make_unique<UI::Animations::CallbackAnimation>(
            [this, oldVal, newVal]() {
                model.updateValue(oldVal, newVal);
            }
        ));

        ctx.animManager.addAnimation(std::move(sequence));
    }

    void HeapController::handleClearAll() {
        if (graph.isAnimating()) {
            model.clear();
            graph.clear();
            return;
        }

        graph.clearEdges();
        int currentSize = static_cast<int>(graph.getNodes().size());
        for (int i = 0; i < currentSize; ++i) {
            graph.removeNodeAt(0);
        }
        model.clear();
    }
    void HeapController::processDeferredActions() {
        if (pendingRebuild && !graph.isAnimating()) {
            pendingRebuild = false;
            rebuildGraphFromModel();
        }
    }

    void HeapController::applyStateToGraph(const std::vector<int>& state) {
        for (int i = 0; i < static_cast<int>(state.size()); ++i) {
            graph.setNodeValueRaw(i, std::to_string(state[i]));
        }
    }

    


} // namespace Controllers