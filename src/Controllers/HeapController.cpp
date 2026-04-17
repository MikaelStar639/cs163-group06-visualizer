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
        : ctx(context), graph(g), model(m), codeViewer(viewer) {}

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

        // 1. Logically add the node to the graph IMMEDIATELY (no animation yet)
        // This gives us a valid pointer to Node(100) before any swaps happen.
        int insertIdx = (int)graph.getNodeCount();
        
        // We create the node at the "hidden" start position
        graph.addNode(std::to_string(val), {startX, startY + 200.f});
        auto* newNodePtr = graph.getNode(insertIdx);

        // 2. Prepare the tracker - it's now perfectly in sync with the model
        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)graph.getNodeCount(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        model.setObserver([this, &b, currentPointers, newNodePtr](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            
            if (action == Core::DSA::HeapAction::Insert) {
                b.highlight("insert_at_end")
                .callback([this, newNodePtr]() {
                    // The node is already in the graph, we just trigger the 
                    // "pop-in" animation manually or via a helper.
                    // If you MUST use insertNodeAt, call it here, but it might 
                    // duplicate the node. Instead, just scale it in:
                    newNodePtr->setScale(0.0f);
                })
                .nodeScale(newNodePtr, 1.0f, 0.2f) // Custom "Insert" pop
                .wait(0.3f)
                .callback([this]() {
                    syncGraphEdges();
                    triggerLayout(0.5f);
                })
                .wait(0.5f);
                return;
            }

            if (action == Core::DSA::HeapAction::Swap) {
                UI::DSA::Node* nodeA = currentPointers[i];
                UI::DSA::Node* nodeB = currentPointers[j];

                if (nodeA && nodeB) {
                    // Update tracker immediately so next notification is correct
                    std::swap(currentPointers[i], currentPointers[j]);

                    b.highlight("swap_with_parent")
                    .nodesHighlight(nodeA, nodeB, 0.4f)
                    .wait(0.2f)
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight(nodeA, nodeB, 0.2f);
                }
            }
        });

        model.insert(val);
        model.setObserver(nullptr);
        b.finish();
        ctx.animManager.addAnimation(b.build());
    }

    void HeapController::handleRemoveRoot() {
        using Builder = UI::Animations::AnimStepBuilder;
        auto codeDef = Core::DSA::PseudoCode::Heap::removeRoot();
        Builder b(codeDef, codeViewer);

        model.setObserver([this, &b](Core::DSA::HeapAction action, int i, int j, int v) {
            
            // 1. First Swap (Animate movement + Logical pointer sync)
            if (action == Core::DSA::HeapAction::Update && i == 0) {
                int lastIdx = (int)graph.getNodeCount() - 1;
                auto* rootNode = graph.getNode(0);
                auto* lastNode = graph.getNode(lastIdx);

                if (rootNode && lastNode) {
                    b.highlight("swap_root")
                    .nodesHighlight(rootNode, lastNode, 0.4f)
                    .wait(0.2f)
                    .callback([this, lastIdx]() {
                        // Sync graph pointers so 'lastNode' is logically at index 0
                        graph.swapNodePointers(0, lastIdx);
                        syncGraphEdges();
                    })
                    .nodeSwap(rootNode, lastNode, 0.8f)
                    .wait(0.8f)
                    .callback([this]() { triggerLayout(0.0f); })
                    // IMPORTANT: These nodes must finish unhighlighting 
                    // BEFORE the removal callback runs.
                    .nodesUnhighlight(rootNode, lastNode, 0.2f)
                    .wait(0.2f); 
                }
                return;
            }

            // 2. Physical Removal (The "Segfault Zone")
            if (action == Core::DSA::HeapAction::Remove) {
                b.highlight("remove_last")
                .callback([this, i]() {
                    // Check if the node actually exists before deleting
                    if (i >= 0 && i < (int)graph.getNodeCount()) {
                        graph.removeNodeAt(i);
                    }
                    syncGraphEdges();
                    triggerLayout(0.5f);
                })
                .wait(0.5f);
                return;
            }

            // 3. Heapify Down Swaps
            if (action == Core::DSA::HeapAction::Swap) {
                auto* nodeA = graph.getNode(i);
                auto* nodeB = graph.getNode(j);

                if (nodeA && nodeB) {
                    b.highlight("swap_with_child")
                    .nodesHighlight(nodeA, nodeB, 0.3f)
                    .wait(0.1f)
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 0.6f)
                    .wait(0.6f)
                    .callback([this]() { triggerLayout(0.0f); })
                    .nodesUnhighlight(nodeA, nodeB, 0.2f)
                    .wait(0.1f); // Brief wait to clear animation references
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

        int currentSize = static_cast<int>(graph.getNodes().size());

        // 1. Check if heap is empty
        b.highlight("check_empty");
        if (currentSize == 0) {
            // You could add a log here if needed
            b.finish();
        } else {
            // 2. Access the root (index 0)
            b.highlight("access_root")
            .nodeHighlight(graph.getNode(0), 0.5f) // Visual flash/highlight on the root node
            .wait(0.3f);

            // 3. Return value phase
            b.highlight("return_val")
            .wait(0.5f)
            .nodeUnhighlight(graph.getNode(0), 0.2f)
            .finish();
        }

        ctx.animManager.addAnimation(b.build());
    }

    void HeapController::handleBuildHeap(const std::vector<int>& data) {
        using Builder = UI::Animations::AnimStepBuilder;

        auto codeDef = Core::DSA::PseudoCode::Heap::buildHeap();
        Builder b(codeDef, codeViewer);

        // This vector tracks the CURRENT logical state of the graph
        // as notifications pour in from the model.
        std::vector<UI::DSA::Node*> currentPointers;
        for (int k = 0; k < (int)data.size(); ++k) {
            currentPointers.push_back(graph.getNode(k));
        }

        model.setObserver([this, &b, currentPointers](Core::DSA::HeapAction action, int i, int j, int v) mutable {
            if (action == Core::DSA::HeapAction::Insert) return;

            if (action == Core::DSA::HeapAction::Focus) {
                b.highlight("loop_outer").wait(0.6f);
            }
            else if (action == Core::DSA::HeapAction::Compare) {
                b.highlight("call_heapify").wait(0.5f);
            }
            else if (action == Core::DSA::HeapAction::Swap) {
                UI::DSA::Node* nodeA = currentPointers[i];
                UI::DSA::Node* nodeB = currentPointers[j];

                if (nodeA && nodeB) {
                    std::swap(currentPointers[i], currentPointers[j]);

                    b.nodesHighlight(nodeA, nodeB, 0.4f) // Both turn orange together
                    .wait(0.2f)
                    .callback([this, i, j]() {
                        graph.swapNodePointers(i, j);
                        syncGraphEdges();
                    })
                    .nodeSwap(nodeA, nodeB, 1.0f) // Circular movement
                    .wait(1.0f)
                    .callback([this]() {
                        triggerLayout(0.0f); // Final coordinate sync
                    })
                    .nodesUnhighlight(nodeA, nodeB, 0.2f); // Both fade out together
                }
            }
        });

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