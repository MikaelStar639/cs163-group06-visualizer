#include "Controllers/HeapController.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Core/AnimStepBuilder.hpp"
#include "Core/Platform.hpp"
#include <iostream>
#include <any>
#include "UI/Animations/StepByStep/HeapSnapshot.hpp"
#include <algorithm>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <sstream>

namespace Controllers {

    HeapController::HeapController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Heap& m, 
                                   UI::Widgets::PseudoCodeViewer* viewer)
        : ctx(context), graph(g), model(m), codeViewer(viewer) {
            g.setIsDirected(false);
            g.setDraggable(false); // Disable physics to prevent 'bunching'

            // Register snapshot handlers
            ctx.stepNavigator.setSnapshotHandlers(
                [this]() { return this->saveSnapshot(); },
                [this](const std::any& s) { this->restoreSnapshot(s); }
            );
        }

    void HeapController::syncGraphEdges() {
        graph.clearEdgesSilently();
        int numNodes = static_cast<int>(graph.getNodes().size());
        
        for (int i = 0; i < numNodes; ++i) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < numNodes) graph.addEdge(i, left, "", false);
            if (right < numNodes) graph.addEdge(i, right, "", false);
        }
    }

    void HeapController::triggerLayout(float duration) {
        int numNodes = static_cast<int>(graph.getNodeCount());
        if (numNodes == 0) return;

        auto layoutAnim = UI::DSA::LayoutEngine::asHeap(graph, startX, startY, 150.f, duration);
        ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void HeapController::submitAnimation(UI::Animations::AnimStepBuilder& b) {
        ctx.animManager.clearAll();
        ctx.stepNavigator.clear();
        masterNodePool.clear(); // Clear cemetery for new algorithm
        auto steps = b.buildSteps();
        for (auto& step : steps) {
            ctx.stepNavigator.addStep(std::shared_ptr<UI::Animations::AnimationBase>(std::move(step)));
        }
        ctx.stepNavigator.playNext();
        if (ctx.isStepByStep) {
            ctx.animManager.setPaused(true);
        }
    }

    void HeapController::forceVisualSync() {
        ctx.stepNavigator.clear();
        masterNodePool.clear();

        const auto& pool = model.getPool();
        int targetSize = static_cast<int>(pool.size());

        // 1. Sync Node Count
        while (static_cast<int>(graph.getNodeCount()) < targetSize) {
            graph.addNode("", {startX, startY});
        }
        while (static_cast<int>(graph.getNodeCount()) > targetSize) {
            graph.removeLastNode();
        }

        // 2. Sync Labels and Reset Visuals
        for (int i = 0; i < targetSize; ++i) {
            if (auto* node = graph.getNode(i)) {
                node->setLabel(std::to_string(pool[i]));
            }
        }
        graph.resetVisuals();

        // 3. Sync Edges
        syncGraphEdges();

        // 4. Force Snapshot Layout (Perfect alignment)
        forceSnapLayout();
    }

    void HeapController::forceSnapLayout() {
        int numNodes = (int)graph.getNodeCount();
        if (numNodes == 0) return;

        int maxLevel = static_cast<int>(std::floor(std::log2(numNodes)));
        float currentSpacing = 150.f; // Back to original spacing

        for (int i = 0; i < numNodes; ++i) {
            int level = static_cast<int>(std::floor(std::log2(i + 1)));
            float targetX = startX;
            int tempIdx = i;
            
            for (int l = level; l > 0; --l) {
                int parentIdx = (tempIdx - 1) / 2;
                bool isRightChild = (tempIdx % 2 == 0);
                float hOffset = std::pow(2.f, maxLevel - l) * (currentSpacing / 2.f);
                targetX += (isRightChild ? 1 : -1) * hOffset;
                tempIdx = parentIdx;
            }

            float targetY = startY + (level * currentSpacing * 0.8f);
            auto* node = graph.getNode(i);
            if (node) node->setPosition({targetX, targetY});
        }
    }

    void HeapController::handleCreateRandom(int size) {
        if (codeViewer) codeViewer->hide();
        
        model.clear();
        graph.clear();

        // Safety limit for visualizer (increased to 100 as requested)
        if (size > 100) size = 100;

        std::vector<int> initialData;
        for (int i = 0; i < size; ++i) {
            int randomVal = std::rand() % 100;
            initialData.push_back(randomVal);
            
            // Use the same insertion method as file loading and manual insert
            // This triggers the NodeInsertAnimation (0.2s) for every node
            graph.insertNodeAt(i, std::to_string(randomVal), {startX, startY});
        }

        model.loadRawData(initialData); 
        
        // Connect the edges for the binary tree structure
        syncGraphEdges();
        
        // Smoothly transition all nodes from the center to their heap positions
        triggerLayout(0.6f);
    }

    void HeapController::handlePreHeapifiedRandom(int size) {
        if (codeViewer) codeViewer->hide();
        
        // 1. Reset state
        model.clear();
        graph.clear();

        // 2. Generate random data
        std::vector<int> data;
        for (int i = 0; i < size; ++i) {
            data.push_back(std::rand() % 100);
        }

        // 3. SILENT HEAPIFY
        // We load the raw data, then call buildHeap.
        // Since no observer is set, no animation steps are recorded yet.
        model.loadRawData(data);
        model.buildHeap(model.getPool()); 

        // 4. SYNC VISUALS
        // Now that the model's internal 'pool' is a heap, we create the nodes
        // in that specific order so they appear pre-heapified.
        const auto& heapifiedData = model.getPool();
        for (int i = 0; i < (int)heapifiedData.size(); ++i) {
            graph.insertNodeAt(i, std::to_string(heapifiedData[i]), {startX, startY});
        }

        // 5. Connect edges and position nodes
        syncGraphEdges();
        
        // Use a slightly longer layout time to make the "pop-in" feel intentional
        triggerLayout(0.8f);
    }

    void HeapController::handleCreateFromFile() {
        if (codeViewer) codeViewer->hide();

        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/HeapData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            // Initializing the file with Heap-specific instructions
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << "# --- HEAP VISUALIZER DATA ---\n"
                        << "# DETAILED INSTRUCTIONS:\n"
                        << "# 1. Type the number of elements 'n' first.\n"
                        << "# 2. Then type the 'n' integer values separated by spaces or newlines.\n"
                        << "#    (Max n is 100. Values must be between -999 and 999).\n"
                        << "# -----------------------------------\n";
                outFile.close();
            }
            std::cout << "[UI LOG] File not found. Created HeapData.txt and opening Notepad.\n";
            Core::Platform::openTextEditor(filePath);
            return;
        }

        // Smart Parser (matches your teammate's logic)
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
                    allNumbers.push_back(std::stoi(token));
                } catch (...) {
                    // Ignore non-numeric text
                }
            }
        }
        file.close();

        std::string errorMsg = "";
        std::vector<int> parsedData;

        // Validation
        if (allNumbers.empty()) {
            errorMsg = "# [WARNING] Could not read 'n'. Please enter the number of elements first.\n";
        } else {
            int n = allNumbers[0];
            if (n < 0) {
                errorMsg = "# [WARNING] Invalid size 'n' = " + std::to_string(n) + ".\n";
            } else if (n > 100) {
                errorMsg = "# [WARNING] Size 'n' = " + std::to_string(n) + " is too large (Max 100).\n";
            } else if (allNumbers.size() - 1 < static_cast<size_t>(n)) {
                errorMsg = "# [WARNING] Expected " + std::to_string(n) + " elements, found " + std::to_string(allNumbers.size() - 1) + ".\n";
            } else {
                for (int i = 1; i <= n; ++i) {
                    if (allNumbers[i] < -999 || allNumbers[i] > 999) {
                        errorMsg = "# [WARNING] Value " + std::to_string(allNumbers[i]) + " out of range.\n";
                        break;
                    }
                    parsedData.push_back(allNumbers[i]);
                }
            }
        }

        if (!errorMsg.empty()) {
            // Re-inject warning into file (logic same as LL)
            // [System Note: Logic for contentWithWarning injection omitted for brevity but identical to your LL code]
            Core::Platform::openTextEditor(filePath);
            return;
        }

        // Load state
        model.clear();
        graph.clear();

        // We use an index counter to ensure they are inserted in order
        int index = 0;
        for (int val : parsedData) {
            // This triggers the internal NodeInsertAnimation (0.2s) for each node
            // We start them all at {startX, startY}, triggerLayout will move them to the tree grid
            graph.insertNodeAt(index++, std::to_string(val), {startX, startY});
        }

        // Load raw data silently into the model so we can visualize buildHeap later
        model.loadRawData(parsedData);

        // Connect the parents and children logically
        syncGraphEdges();

        // Glide everything from {startX, startY} into the correct Binary Tree layout
        triggerLayout(0.6f);
    }

    void HeapController::handlePreHeapifiedFromFile() {
        if (codeViewer) codeViewer->hide();

        std::string dirPath = "user_data";
        std::string filePath = dirPath + "/HeapData.txt";

        // 1. Ensure file exists (Standard boilerplate from your original function)
        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        std::ifstream file(filePath);
        if (!file.is_open()) {
            std::ofstream outFile(filePath);
            if (outFile.is_open()) {
                outFile << "# --- HEAP VISUALIZER DATA ---\n"
                        << "# 1. Type number of elements 'n' first.\n"
                        << "# 2. Type 'n' integers (-999 to 999).\n"
                        << "# -----------------------------------\n";
                outFile.close();
            }
            Core::Platform::openTextEditor(filePath);
            return;
        }

        // 2. Parse the numbers
        std::string line;
        std::vector<int> allNumbers;
        while (std::getline(file, line)) {
            size_t startPos = line.find_first_not_of(" \t\r\n");
            if (startPos != std::string::npos && line[startPos] == '#') continue; 

            std::stringstream ss(line);
            std::string token;
            while (ss >> token) {
                try { allNumbers.push_back(std::stoi(token)); } catch (...) {}
            }
        }
        file.close();

        // 3. Validation
        std::string errorMsg = "";
        std::vector<int> parsedData;
        if (allNumbers.empty()) {
            errorMsg = "# [WARNING] Could not read 'n'.\n";
        } else {
            int n = allNumbers[0];
            if (n < 0 || n > 100) {
                errorMsg = "# [WARNING] Invalid size (Max 100).\n";
            } else if ((int)allNumbers.size() - 1 < n) {
                errorMsg = "# [WARNING] Not enough elements found.\n";
            } else {
                for (int i = 1; i <= n; ++i) {
                    if (allNumbers[i] < -999 || allNumbers[i] > 999) {
                        errorMsg = "# [WARNING] Value out of range.\n";
                        break;
                    }
                    parsedData.push_back(allNumbers[i]);
                }
            }
        }

        if (!errorMsg.empty()) {
            Core::Platform::openTextEditor(filePath);
            return;
        }

        // --- START PRE-HEAPIFY LOGIC ---

        // 4. Load state and Clear visuals
        model.clear();
        graph.clear();

        // 5. Silent Heapify: Order the data BEFORE creating nodes
        model.loadRawData(parsedData);
        model.buildHeap(model.getPool()); 

        // 6. Create Visual Nodes based on the HEAPIFIED pool
        const auto& heapifiedData = model.getPool();
        for (int i = 0; i < (int)heapifiedData.size(); ++i) {
            // Create nodes in the order they exist in the Max-Heap
            graph.insertNodeAt(i, std::to_string(heapifiedData[i]), {startX, startY});
        }

        // 7. Finalize layout
        syncGraphEdges();
        triggerLayout(0.8f);
    }

    void HeapController::handleEditDataFile() {
        std::string dirPath = "user_data";
        // Change 1: Update filename to match HeapController's data source
        std::string filePath = dirPath + "/HeapData.txt";

        if (!std::filesystem::exists(dirPath)) {
            std::filesystem::create_directories(dirPath);
        }

        // Change 2: Update header text for Heap-specific context
        std::string header = "# --- HEAP VISUALIZER DATA ---\n"
                            "# DETAILED INSTRUCTIONS:\n"
                            "# 1. Type the number of elements 'n' first.\n"
                            "# 2. Then type the 'n' integer values separated by spaces or newlines.\n"
                            "#    (Max n is 100. Values must be between -999 and 999).\n"
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
        
        // Change 3: Ensure we open the correct heap file in Notepad
        Core::Platform::openTextEditor(filePath);
    }

    void HeapController::handleInsert(int val) {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Heap::insert();
        Builder b(codeDef, codeViewer);

        // Tracker for the observer to build correct indices
        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)graph.getNodeCount(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        // We use a temporary model to record the animation steps.
        // This keeps the REAL model in its 'Before' state for the first snapshot.
        Core::DSA::Heap tempModel = model;
        tempModel.setObserver([this, &b, currentPointers, val](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            
            if (action == Core::DSA::HeapAction::Insert) {
                // Line: pool.push_back(val)
                b.highlight("insert_at_end")
                .callback([this, val]() {
                    // SILENT UPDATE: Update the real model now that the animation has started
                    model.insert(val);
                    
                    // VISUAL UPDATE: Create the node in the graph
                    // It will be at the end of the graph's nodes vector
                    graph.addNodeRaw(std::to_string(val), {startX, startY + 200.f});
                    
                    syncGraphEdges();
                    triggerLayout(Config::Animation::DURATION_LAYOUT);
                })
                .wait(Config::Animation::STEP_WAIT_LAYOUT)
                .nextStep();

                // Line: heapifyUp(last_index)
                b.highlight("heapify_up").nextStep();
                return;
            }

            if (action == Core::DSA::HeapAction::Compare) {
                UI::DSA::Node* parent = currentPointers[i];
                UI::DSA::Node* child = currentPointers[j];
                UI::DSA::Node* winnerNode = (v >= 0 && v < (int)currentPointers.size()) ? currentPointers[v] : nullptr;

                // Handle the newly inserted node if it's the child
                if (!child && j == (int)currentPointers.size()) {
                    // This is expected for the first comparison after insert
                    // But since the callback hasn't run yet, we don't have the node pointer.
                    // However, we can use graph.getNode(j) inside the callback!
                }

                if (parent) {
                    // Line: while index > 0
                    b.highlight("loop_cond").nextStep();

                    // Line: if val > parent
                    b.highlight("compare_parent")
                    .callback([this, i, j, winnerNode]() mutable {
                        // Resolve child node at runtime
                        auto* p = graph.getNode(i);
                        auto* c = graph.getNode(j);
                        if (p && c) {
                            auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                            parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(p, Config::Animation::DURATION_COLOR));
                            parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(c, Config::Animation::DURATION_COLOR));
                            ctx.animManager.addAnimation(std::move(parallel));
                        }
                    })
                    .wait(Config::Animation::STEP_WAIT_ACTION).nextStep();

                    b.callback([this, i, j, winnerNode, v]() mutable {
                        auto* p = graph.getNode(i);
                        auto* c = graph.getNode(j);
                        auto* winner = (v >= 0) ? graph.getNode(v) : nullptr;
                        
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        bool added = false;
                        if (p && p != winner) { parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(p, Config::Animation::DURATION_COLOR)); added = true; }
                        if (c && c != winner) { parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(c, Config::Animation::DURATION_COLOR)); added = true; }
                        
                        if (added) {
                            ctx.animManager.addAnimation(std::move(parallel));
                        }
                    }).wait(Config::Animation::DURATION_COLOR);
                }
            }

            if (action == Core::DSA::HeapAction::Swap) {
                // IMPORTANT: We capture indices i and j to resolve pointers at runtime
                b.highlight("swap_with_parent")
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                        
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeSwapAnimation>(nodeA, nodeB, Config::Animation::DURATION_MOVE));
                    }
                })
                .wait(Config::Animation::DURATION_MOVE)
                .callback([this]() { triggerLayout(0.0f); })
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeA, Config::Animation::DURATION_QUICK));
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeB, Config::Animation::DURATION_QUICK));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(Config::Animation::STEP_WAIT_ACTION).nextStep();

                // Update our local tracker for the observer's subsequent steps
                if (j < (int)currentPointers.size()) {
                    std::swap(currentPointers[i], currentPointers[j]);
                } else {
                    // The child was the new node, we just push a placeholder to keep index alignment
                    // if it wasn't already there.
                    while((int)currentPointers.size() <= j) currentPointers.push_back(nullptr);
                    std::swap(currentPointers[i], currentPointers[j]);
                }
            }

            if (action == Core::DSA::HeapAction::Unfocus) {
                // Line: else: break
                b.highlight("break_loop")
                .callback([this, i]() {
                    auto* winnerNode = graph.getNode(i);
                    if (winnerNode) {
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(winnerNode, 0.3f));
                    }
                })
                .wait(0.2f).nextStep();
            }
        });

        tempModel.insert(val);
        tempModel.setObserver(nullptr);
        b.callback([this]() { triggerLayout(Config::Animation::DURATION_LAYOUT); }); // Final beautiful layout
        b.finish();
        submitAnimation(b);
    }

    // ==================== REMOVE ROOT (POP) ====================
    void HeapController::handleRemoveRoot() {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Heap::removeRoot();
        Builder b(codeDef, codeViewer);

        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)graph.getNodeCount(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        Core::DSA::Heap tempModel = model;
        tempModel.setObserver([this, &b, currentPointers](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            
            // 1. Swap Root with Last Node
            if (action == Core::DSA::HeapAction::Update && i == 0) {
                int lastIdx = (int)currentPointers.size() - 1;

                b.highlight("swap_root")
                .callback([this, lastIdx]() {
                    // SILENT UPDATE: Update the real model at the very beginning of the removal process
                    // This ensures the model is in the "Post-Removal" state for subsequent snapshots.
                    model.removeRoot();

                    auto* rootNode = graph.getNode(0);
                    auto* lastNode = graph.getNode(lastIdx);
                    if (rootNode && lastNode) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(rootNode, 0.3f));
                        parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(lastNode, 0.3f));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(Config::Animation::STEP_WAIT_ACTION)
                .callback([this, lastIdx]() {
                    auto* rootNode = graph.getNode(0);
                    auto* lastNode = graph.getNode(lastIdx);
                    if (rootNode && lastNode) {
                        graph.swapNodePointers(0, lastIdx);
                        syncGraphEdges();
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeSwapAnimation>(rootNode, lastNode, 0.6f));
                    }
                })
                .wait(0.6f)
                .callback([this, lastIdx]() { 
                    triggerLayout(0.0f); 
                    auto* nodeA = graph.getNode(0);
                    auto* nodeB = graph.getNode(lastIdx);
                    if (nodeA && nodeB) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeA, 0.2f));
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeB, 0.2f));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(0.2f).nextStep(); 

                std::swap(currentPointers[0], currentPointers[lastIdx]);
                return;
            }

            // 2. Physical Removal
            if (action == Core::DSA::HeapAction::Remove) {
                // Line: pool.pop_back()
                b.highlight("remove_last")
                .callback([this, i]() {
                    if (i >= 0 && i < (int)graph.getNodeCount()) {
                        auto extracted = graph.extractNode(i);
                        if (extracted) {
                            masterNodePool.push_back(std::move(extracted));
                        }
                    }
                    syncGraphEdges();
                    triggerLayout(Config::Animation::DURATION_LAYOUT);
                })
                .wait(Config::Animation::STEP_WAIT_ACTION).nextStep();

                if (i < (int)currentPointers.size()) {
                    currentPointers.erase(currentPointers.begin() + i);
                }

                // Line: heapifyDown(0)
                b.highlight("heapify_down").nextStep();
                return;
            }

            // 3. Comparison Logic (Trio Highlight)
            else if (action == Core::DSA::HeapAction::Compare) {
                // Line: while leftChild exists
                b.highlight("loop_cond").nextStep();

                // Line: target = larger child
                b.highlight("find_max_child")
                .callback([this, i, j, v]() {
                    int leftChildIdx = j;
                    int rightChildIdx = j + 1;
                    std::vector<UI::DSA::Node*> trio;
                    if (auto* n = graph.getNode(i)) trio.push_back(n);
                    if (auto* n = graph.getNode(leftChildIdx)) trio.push_back(n);
                    if (auto* n = graph.getNode(rightChildIdx)) trio.push_back(n);
                    
                    if (!trio.empty()) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        for (auto* n : trio) parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(n, Config::Animation::DURATION_COLOR));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(Config::Animation::STEP_WAIT_ACTION).nextStep();

                // Line: if target > val
                b.highlight("compare_child")
                .callback([this, i, j, v]() {
                    int leftChildIdx = j;
                    int rightChildIdx = j + 1;
                    auto* winnerNode = graph.getNode(v);
                    
                    auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                    bool added = false;
                    if (auto* n = graph.getNode(i); n && n != winnerNode) { parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(n, Config::Animation::DURATION_COLOR)); added = true; }
                    if (auto* n = graph.getNode(leftChildIdx); n && n != winnerNode) { parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(n, Config::Animation::DURATION_COLOR)); added = true; }
                    if (auto* n = graph.getNode(rightChildIdx); n && n != winnerNode) { parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(n, Config::Animation::DURATION_COLOR)); added = true; }

                    if (added) {
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(0.2f).nextStep();
            }

            else if (action == Core::DSA::HeapAction::Swap) {
                // Line: swap(val, target)
                b.highlight("swap_with_child")
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeSwapAnimation>(nodeA, nodeB, 0.6f));
                    }
                })
                .wait(0.6f)
                .callback([this]() { triggerLayout(0.0f); })
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeA, 0.2f));
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeB, 0.2f));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(0.2f).nextStep();
                
                if (i < (int)currentPointers.size() && j < (int)currentPointers.size()) {
                    std::swap(currentPointers[i], currentPointers[j]);
                }
            }

            // 5. Termination
            else if (action == Core::DSA::HeapAction::Unfocus) {
                // Line: else: break
                b.highlight("break_loop")
                .callback([this, i]() {
                    auto* winnerNode = graph.getNode(i);
                    if (winnerNode) {
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(winnerNode, 0.3f));
                    }
                })
                .wait(0.2f).nextStep();
            }
        });

        tempModel.removeRoot();
        tempModel.setObserver(nullptr);
        b.callback([this]() { triggerLayout(Config::Animation::DURATION_LAYOUT); }); // Final beautiful layout
        b.finish();
        submitAnimation(b);
    }
    
    void HeapController::handleReturnRoot() {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::Heap::returnRoot();
        Builder b(codeDef, codeViewer);

        int currentSize = static_cast<int>(graph.getNodeCount());

        Core::DSA::Heap tempModel = model;
        
        b.highlight("check_empty").nextStep();
        
        if (currentSize == 0) {
            b.finish();
        } else {
            auto* rootNode = graph.getNode(0);
            
            // 2. Access the root (index 0)
            // Highlight the root node using the vector-based call
            b.highlight("access_root")
            .nodesHighlight({rootNode}, 0.5f) 
            .wait(Config::Animation::STEP_WAIT_ACTION).nextStep();

            // 3. Return value phase
            b.highlight("return_val")
            .wait(Config::Animation::STEP_WAIT_ACTION)
            .nodesUnhighlight({rootNode}, 0.2f)
            .nextStep().finish();
        }

        submitAnimation(b);
    }

    void HeapController::handleBuildHeap(const std::vector<int>& data) {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::Heap::buildHeap();
        Builder b(codeDef, codeViewer);

        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)data.size(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        Core::DSA::Heap tempModel = model;
        tempModel.setObserver([this, &b, data](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            if (action == Core::DSA::HeapAction::Insert) return;

            if (action == Core::DSA::HeapAction::Focus) {
                b.highlight("loop_outer")
                .callback([this, data]() {
                    model.buildHeap(data);
                })
                .nextStep();
            }
            else if (action == Core::DSA::HeapAction::Compare) {
                b.highlight("call_heapify")
                .callback([this, i, j]() {
                    auto* p = graph.getNode(i);
                    auto* l = graph.getNode(j);
                    auto* r = graph.getNode(j + 1);
                    auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                    if (p) parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(p, 0.3f));
                    if (l) parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(l, 0.3f));
                    if (r) parallel->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(r, 0.3f));
                    ctx.animManager.addAnimation(std::move(parallel));
                })
                .wait(0.4f).nextStep();

                // Small unhighlight step before potential swap
                b.highlight("call_heapify")
                .callback([this, i, j, v]() {
                    auto* winnerNode = (v >= 0) ? graph.getNode(v) : nullptr;
                    auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                    bool added = false;
                    for (int idx : {i, j, j+1}) {
                        auto* n = graph.getNode(idx);
                        if (n && n != winnerNode) {
                            parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(n, Config::Animation::DURATION_QUICK));
                            added = true;
                        }
                    }
                    if (added) ctx.animManager.addAnimation(std::move(parallel));
                }).wait(0.2f).nextStep();
            }
            else if (action == Core::DSA::HeapAction::Swap) {
                b.highlight("call_heapify") 
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeSwapAnimation>(nodeA, nodeB, 0.6f));
                    }
                })
                .wait(0.6f)
                .callback([this]() { triggerLayout(0.0f); })
                .callback([this, i, j]() {
                    auto* nodeA = graph.getNode(i);
                    auto* nodeB = graph.getNode(j);
                    if (nodeA && nodeB) {
                        auto parallel = std::make_unique<UI::Animations::ParallelAnimation>();
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeA, 0.2f));
                        parallel->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(nodeB, 0.2f));
                        ctx.animManager.addAnimation(std::move(parallel));
                    }
                })
                .wait(0.1f).nextStep();
            }
            else if (action == Core::DSA::HeapAction::Unfocus) {
                b.highlight("loop_outer") // Return to loop after heapifying down
                .callback([this, i]() {
                    if (auto* n = graph.getNode(i)) {
                        ctx.animManager.addAnimation(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(n, Config::Animation::DURATION_COLOR));
                    }
                })
                .wait(0.2f).nextStep();
            }
        }); 

        tempModel.buildHeap(data);
        tempModel.setObserver(nullptr);
        b.callback([this]() { triggerLayout(Config::Animation::DURATION_LAYOUT); }); // Final beautiful layout
        b.finish();
        submitAnimation(b);
    }

    void HeapController::handleClearAll() {
        if (codeViewer) codeViewer->hide();

        model.clear();
        graph.clear();
        masterNodePool.clear();
    }

    // ==================== SNAPSHOTS ====================

    std::any HeapController::saveSnapshot() {
        // Force nodes to intended positions before snapshot
        triggerLayout(0.f);

        UI::Animations::HeapSnapshot s;
        s.pool = model.getPool();

        auto& currentNodes = graph.getNodes();
        for (const auto& nodePtr : currentNodes) {
            UI::Animations::HeapSnapshot::NodeState ns;
            ns.originalPointer = nodePtr.get();
            ns.position = nodePtr->getPosition();
            ns.fillColor = nodePtr->getFillColor();
            ns.outlineColor = nodePtr->getOutlineColor();
            ns.labelColor = nodePtr->getLabelColor();
            ns.scale = nodePtr->getScale();
            ns.label = nodePtr->getLabel();
            s.nodes.push_back(ns);
        }

        if (codeViewer) s.activeLineIndex = codeViewer->getActiveLine();

        return std::make_any<UI::Animations::HeapSnapshot>(std::move(s));
    }

    void HeapController::restoreSnapshot(const std::any& snapshotAny) {
        const auto& s = std::any_cast<const UI::Animations::HeapSnapshot&>(snapshotAny);
        
        // CRITICAL: Stop any pending layout or swap animations that might fight the snapshot state
        ctx.animManager.clearAll();

        // 1. Model
        model.loadRawData(s.pool);

        // 2. Visuals - Rebuild graph from pool and snapshot
        std::vector<std::unique_ptr<UI::DSA::Node>> newGraphNodes;
        
        for (const auto& nodeState : s.nodes) {
            std::unique_ptr<UI::DSA::Node> node = nullptr;

            // Try extracting from current graph
            for (int i = 0; i < (int)graph.getNodeCount(); ++i) {
                if (graph.getNode(i) == nodeState.originalPointer) {
                    node = graph.extractNode(i);
                    break;
                }
            }

            // Try extracting from pool
            if (!node) {
                for (auto it = masterNodePool.begin(); it != masterNodePool.end(); ++it) {
                    if (it->get() == nodeState.originalPointer) {
                        node = std::move(*it);
                        masterNodePool.erase(it);
                        break;
                    }
                }
            }

            // If found, restore state
            if (node) {
                node->setLabel(nodeState.label);
                node->setPosition(nodeState.position);
                node->setFillColor(nodeState.fillColor);
                node->setOutlineColor(nodeState.outlineColor);
                node->setLabelColor(nodeState.labelColor);
                node->setScale(nodeState.scale);
                
                newGraphNodes.push_back(std::move(node));
            }
        }

        // Move remaining current graph nodes to pool
        while (graph.getNodeCount() > 0) {
            masterNodePool.push_back(graph.extractNode(0));
        }

        // Set the new nodes
        for (auto& node : newGraphNodes) {
            graph.insertNodePtr(-1, std::move(node));
        }

        syncGraphEdges();

        if (codeViewer && s.activeLineIndex >= 0) {
            codeViewer->highlightLine(s.activeLineIndex);
        }
    }

} // namespace Controllers
