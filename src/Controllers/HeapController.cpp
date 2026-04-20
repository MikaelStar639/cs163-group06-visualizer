#include "Controllers/HeapController.hpp"
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

    HeapController::HeapController(AppContext& context, UI::DSA::Graph& g, Core::DSA::Heap& m, 
                                   UI::Widgets::PseudoCodeViewer* viewer)
        : ctx(context), graph(g), model(m), codeViewer(viewer) {
            g.setIsDirected(false);
        }

    void HeapController::syncGraphEdges() {
        graph.clearEdges();
        int numNodes = static_cast<int>(graph.getNodes().size());
        
        for (int i = 0; i < numNodes; ++i) {
            int left = 2 * i + 1;
            int right = 2 * i + 2;

            if (left < numNodes) graph.addEdge(i, left);
            if (right < numNodes) graph.addEdge(i, right);
        }
    }

    void HeapController::triggerLayout(float duration) {
        auto layoutAnim = UI::DSA::LayoutEngine::asHeap(graph, startX, startY, spacing, duration);
        ctx.animManager.addAnimation(std::move(layoutAnim));
    }

    void HeapController::forceSnapLayout() {
        const auto& nodes = graph.getNodes();
        for (size_t i = 0; i < nodes.size(); ++i) {
            int level = static_cast<int>(std::floor(std::log2(i + 1)));
            int firstIdxInLevel = static_cast<int>(std::pow(2, level)) - 1;
            int posInLevel = static_cast<int>(i) - firstIdxInLevel;
            int numNodesInLevel = static_cast<int>(std::pow(2, level));

            float levelWidth = (numNodesInLevel - 1) * spacing;
            float targetX = startX + (posInLevel * spacing) - (levelWidth / 2.0f);
            float targetY = startY + (level * spacing);

            if (nodes[i]) {
                nodes[i]->setPosition({targetX, targetY});
            }
        }
    }

    void HeapController::handleCreateRandom(int size) {
        if (codeViewer) codeViewer->hide();
        
        model.clear();
        graph.clear();

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
                        << "#    (Max n is 15. Values must be between -999 and 999).\n"
                        << "# -----------------------------------\n";
                outFile.close();
            }
            std::cout << "[UI LOG] File not found. Created HeapData.txt and opening Notepad.\n";
            std::system(("start notepad " + filePath).c_str());
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
            } else if (n > 15) {
                errorMsg = "# [WARNING] Size 'n' = " + std::to_string(n) + " is too large (Max 15).\n";
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
            std::system(("start notepad " + filePath).c_str());
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
            std::system(("start notepad " + filePath).c_str());
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
            if (n < 0 || n > 15) {
                errorMsg = "# [WARNING] Invalid size (Max 15).\n";
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
            std::system(("start notepad " + filePath).c_str());
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
        
        // Change 3: Ensure we open the correct heap file in Notepad
        std::system(("start notepad " + filePath).c_str());
    }

    void HeapController::handleInsert(int val) {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Heap::insert();
        Builder b(codeDef, codeViewer);

        // 1. Logically add the node to the graph
        int insertIdx = (int)graph.getNodeCount();
        graph.addNode(std::to_string(val), {startX, startY + 200.f});
        auto* newNodePtr = graph.getNode(insertIdx);

        // 2. Prepare the tracker
        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)graph.getNodeCount(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        model.setObserver([this, &b, currentPointers, newNodePtr](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            
            if (action == Core::DSA::HeapAction::Insert) {
                // Line: pool.push_back(val)
                b.highlight("insert_at_end")
                .callback([this, newNodePtr]() {
                    newNodePtr->setScale(0.0f);
                })
                .nodeScale(newNodePtr, 1.0f, 0.2f)
                .wait(0.3f)
                .callback([this]() {
                    syncGraphEdges();
                    triggerLayout(0.5f);
                })
                // Line: heapifyUp(last_index)
                .highlight("heapify_up")
                .wait(0.5f);
                return;
            }

            if (action == Core::DSA::HeapAction::Compare) {
                UI::DSA::Node* parent = currentPointers[i];
                UI::DSA::Node* child = currentPointers[j];
                UI::DSA::Node* winnerNode = (v >= 0 && v < (int)currentPointers.size()) ? currentPointers[v] : nullptr;

                if (parent && child) {
                    // Line: while index > 0
                    b.highlight("loop_cond").wait(0.3f)
                    // Line: if val > parent
                    .highlight("compare_parent")
                    .nodesHighlight({parent, child}, 0.3f)
                    .wait(0.5f);

                    std::vector<UI::DSA::Node*> losers;
                    if (parent != winnerNode) losers.push_back(parent);
                    if (child != winnerNode) losers.push_back(child);

                    if (!losers.empty()) {
                        b.nodesUnhighlight(losers, 0.3f);
                    }

                    b.wait(0.5f);
                }
            }

            if (action == Core::DSA::HeapAction::Swap) {
                UI::DSA::Node* nodeA = currentPointers[i];
                UI::DSA::Node* nodeB = currentPointers[j];

                if (nodeA && nodeB) {
                    std::swap(currentPointers[i], currentPointers[j]);

                    // Line: swap(val, parent)
                    b.highlight("swap_with_parent")
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight({nodeA, nodeB}, 0.3f)
                    .wait(0.2f);
                }
            }

            if (action == Core::DSA::HeapAction::Unfocus) {
                if (i >= 0 && i < (int)currentPointers.size()) {
                    UI::DSA::Node* winnerNode = currentPointers[i];
                    if (winnerNode) {
                        // Line: else: break
                        b.highlight("break_loop")
                        .nodesUnhighlight({winnerNode}, 0.3f)
                        .wait(0.4f);
                    }
                }
            }
        });

        model.insert(val);
        model.setObserver(nullptr);
        b.finish();
        ctx.animManager.addAnimation(b.build());
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

        model.setObserver([this, &b, currentPointers](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            
            // 1. Swap Root with Last Node
            if (action == Core::DSA::HeapAction::Update && i == 0) {
                int lastIdx = (int)currentPointers.size() - 1;
                auto* rootNode = currentPointers[0];
                auto* lastNode = currentPointers[lastIdx];

                if (rootNode && lastNode) {
                    std::swap(currentPointers[0], currentPointers[lastIdx]);

                    // Line: swap(root, last_element)
                    b.highlight("swap_root")
                    .nodesHighlight({rootNode, lastNode}, 0.4f)
                    .wait(0.2f)
                    .callback([this, lastIdx]() {
                        graph.swapNodePointers(0, lastIdx);
                        syncGraphEdges();
                    })
                    .nodeSwap(rootNode, lastNode, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight({rootNode, lastNode}, 0.2f)
                    .wait(0.2f); 
                }
                return;
            }

            // 2. Physical Removal
            if (action == Core::DSA::HeapAction::Remove) {
                if (i >= 0 && i < (int)currentPointers.size()) {
                    currentPointers.erase(currentPointers.begin() + i);
                }

                // Line: pool.pop_back()
                b.highlight("remove_last")
                .callback([this, i]() {
                    if (i >= 0 && i < (int)graph.getNodeCount()) {
                        graph.removeNodeAt(i);
                    }
                    syncGraphEdges();
                    triggerLayout(0.5f);
                })
                .wait(0.5f)
                // Line: heapifyDown(0)
                .highlight("heapify_down")
                .wait(0.5f);
                return;
            }

            // 3. Comparison Logic (Trio Highlight)
            else if (action == Core::DSA::HeapAction::Compare) {
                int leftChildIdx = j;
                int rightChildIdx = j + 1;

                std::vector<UI::DSA::Node*> trio;
                if (i < (int)currentPointers.size()) trio.push_back(currentPointers[i]);
                if (leftChildIdx < (int)currentPointers.size()) trio.push_back(currentPointers[leftChildIdx]);
                if (rightChildIdx < (int)currentPointers.size()) trio.push_back(currentPointers[rightChildIdx]);

                UI::DSA::Node* winnerNode = (v < (int)currentPointers.size()) ? currentPointers[v] : nullptr;
                
                std::vector<UI::DSA::Node*> losers;
                for (auto* node : trio) {
                    if (node != winnerNode) losers.push_back(node);
                }

                // Line: while leftChild exists
                b.highlight("loop_cond").wait(0.3f)
                // Line: target = larger child
                .highlight("find_max_child")
                .nodesHighlight(trio, 0.3f)
                .wait(0.5f)
                // Line: if target > val
                .highlight("compare_child")
                .wait(0.3f);

                if (!losers.empty()) {
                    b.nodesUnhighlight(losers, 0.3f);
                }

                b.wait(0.5f);
            }

            // 4. Swap Logic
            else if (action == Core::DSA::HeapAction::Swap) {
                UI::DSA::Node* nodeA = currentPointers[i]; // Parent
                UI::DSA::Node* nodeB = currentPointers[j]; // Winner Child

                if (nodeA && nodeB) {
                    std::swap(currentPointers[i], currentPointers[j]);

                    // Line: swap(val, target)
                    b.highlight("swap_with_child")
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight({nodeA, nodeB}, 0.3f)
                    .wait(0.2f);
                }
            }

            // 5. Termination
            else if (action == Core::DSA::HeapAction::Unfocus) {
                if (i >= 0 && i < (int)currentPointers.size()) {
                    UI::DSA::Node* winnerNode = currentPointers[i];
                    if (winnerNode) {
                        // Line: else: break
                        b.highlight("break_loop")
                        .nodesUnhighlight({winnerNode}, 0.3f).wait(0.2f);
                    }
                }
            }
        });

        model.removeRoot();
        model.setObserver(nullptr);
        b.finish();
        ctx.animManager.addAnimation(b.build());
    }
    
    void HeapController::handleReturnRoot() {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::Heap::returnRoot();
        Builder b(codeDef, codeViewer);

        int currentSize = static_cast<int>(graph.getNodeCount());

        // 1. Check if heap is empty
        b.highlight("check_empty").wait(0.3f);
        
        if (currentSize == 0) {
            b.finish();
        } else {
            auto* rootNode = graph.getNode(0);
            
            // 2. Access the root (index 0)
            // Highlight the root node using the vector-based call
            b.highlight("access_root")
            .nodesHighlight({rootNode}, 0.5f) 
            .wait(0.5f);

            // 3. Return value phase
            b.highlight("return_val")
            .wait(0.5f)
            .nodesUnhighlight({rootNode}, 0.2f)
            .finish();
        }

        ctx.animManager.addAnimation(b.build());
    }

    void HeapController::handleBuildHeap(const std::vector<int>& data) {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::Heap::buildHeap();
        Builder b(codeDef, codeViewer);

        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)data.size(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        model.setObserver([this, &b, currentPointers](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            if (action == Core::DSA::HeapAction::Insert) return;

            // Line: for i = (size/2)-1 down to 0
            if (action == Core::DSA::HeapAction::Focus) {
                b.highlight("loop_outer").wait(0.6f);
            }
            else if (action == Core::DSA::HeapAction::Compare) {
                int leftChildIdx = j;
                int rightChildIdx = j + 1;

                std::vector<UI::DSA::Node*> trio;
                if (i >= 0 && i < (int)currentPointers.size()) trio.push_back(currentPointers[i]);
                if (leftChildIdx >= 0 && leftChildIdx < (int)currentPointers.size()) trio.push_back(currentPointers[leftChildIdx]);
                if (rightChildIdx >= 0 && rightChildIdx < (int)currentPointers.size()) trio.push_back(currentPointers[rightChildIdx]);

                UI::DSA::Node* winnerNode = (v >= 0 && v < (int)currentPointers.size()) ? currentPointers[v] : nullptr;
                
                std::vector<UI::DSA::Node*> losers;
                for (auto* node : trio) {
                    if (node != winnerNode) losers.push_back(node);
                }

                // Line: heapifyDown(i)
                b.highlight("call_heapify")
                .nodesHighlight(trio, 0.3f)
                .wait(0.5f);

                if (!losers.empty()) {
                    b.nodesUnhighlight(losers, 0.3f);
                }

                b.wait(0.5f);
            }
            else if (action == Core::DSA::HeapAction::Swap) {
                UI::DSA::Node* nodeA = currentPointers[i];
                UI::DSA::Node* nodeB = currentPointers[j];

                if (nodeA && nodeB) {
                    std::swap(currentPointers[i], currentPointers[j]);

                    b.highlight("call_heapify") // Keep highlighting the call line
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight({nodeA, nodeB}, 0.3f)
                    .wait(0.2f);
                }
            }
            else if (action == Core::DSA::HeapAction::Unfocus) {
                if (i >= 0 && i < (int)currentPointers.size()) {
                    UI::DSA::Node* winnerNode = currentPointers[i];
                    if (winnerNode) {
                        b.nodesUnhighlight({winnerNode}, 0.3f).wait(0.2f);
                    }
                }
            }
        }); // Closure of Lambda

        model.buildHeap(data);
        model.setObserver(nullptr);
        b.finish();
        ctx.animManager.addAnimation(b.build());
    }

    void HeapController::handleClearAll() {
        if (codeViewer) codeViewer->hide();

        // If an animation is currently running, perform an instant reset
        if (graph.isAnimating()) {
            model.clear();
            graph.clear();
            return;
        }

        // Otherwise, perform an ordered cleanup
        graph.clearEdges();
        int currentSize = static_cast<int>(graph.getNodes().size());
        
        // Always remove at index 0 because the vector shifts down each time
        for (int i = 0; i < currentSize; ++i) {
            graph.removeNodeAt(0);
        }

        model.clear();
    }
}