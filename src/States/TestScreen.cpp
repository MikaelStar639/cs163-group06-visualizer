#include "States/TestScreen.hpp"
#include "Core/Constants.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Node/NodeSwapAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/ParallelAnimation.hpp"
#include <iostream>
#include <cstdlib> 
#include <ctime>   
#include <algorithm>

TestScreen::TestScreen(AppContext& context)
    : ctx(context), 
      btnInsert(context, "INSERT NODE", {50.f, 50.f}, {250.f, 60.f}) 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void TestScreen::addNewNode(const std::string &val){
    // ... (Your existing position logic remains the same)
    float viewW = 1600.f;
    float viewH = 900.f;
    float nodeDiameter = Config::UI::NODE_RADIUS * 2.f;
    float nodeOutline  = Config::UI::NODE_OUTLINE_THICKNESS;
    float totalNodeSize = nodeDiameter + nodeOutline * 2.f;
    float padding = 50.f; 
    float xMin = padding;
    float xMax = viewW - totalNodeSize - padding;
    float yMin = 150.f; 
    float yMax = viewH - totalNodeSize - padding;
    float finalX = static_cast<float>(std::rand() % static_cast<int>(xMax - xMin + 1)) + xMin;
    float finalY = static_cast<float>(std::rand() % static_cast<int>(yMax - yMin + 1)) + yMin;

    // 1. Create the new node
    nodes.emplace_back(std::make_unique<UI::DSA::Node>(ctx, val, sf::Vector2f(finalX, finalY)));

    UI::DSA::Node* newNodePtr = nodes.back().get();
    size_t newNodeIndex = nodes.size() - 1;


    if (nodes.size() >= 2) {
        // 1. Add the edge to the vector
        edges.emplace_back(std::make_unique<UI::DSA::Edge>(
            ctx, 
            nodes.end()[-2].get(), 
            nodes[newNodeIndex].get(), 
            true
        ));

        // 2. Get the pointer from the back of the vector (matching your Node pattern)
        UI::DSA::Edge* newEdgePtr = edges.back().get();
        auto addNewEdge = std::make_unique<UI::Animations::SequenceAnimation>();
        addNewEdge->add(std::make_unique<UI::Animations::EdgeInsertAnimation>(newEdgePtr, 0.2f));
        addNewEdge->add(std::make_unique<UI::Animations::NodeInsertAnimation>(newNodePtr, 0.2f));
        ctx.animManager.addAnimation(std::move(addNewEdge));
    } else{
        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeInsertAnimation>(newNodePtr, 0.1f)
        );
    }
    
    drawOrder.push_back(newNodeIndex);
    std::cout << ">>> Node '" << val << "' inserted\n";
}

void TestScreen::handleEvent(const sf::Event& event) {
    // ... (Keep your existing button/keyboard/drag logic exactly as is)
    if (btnInsert.isClicked(event)) addNewNode(std::to_string(std::rand() % 100));
    
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Enter){
            std::cout << ">>> Enter node value: ";
            std::string val;
            std::cin >> val;
            addNewNode(val);
        }
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) {
            // 1. BLOCK SPAM
            if (ctx.animManager.empty()){
                if (nodes.size() >= 2) {
                    ctx.animManager.addAnimation(
                        std::make_unique<UI::Animations::NodeSwapAnimation>(
                            nodes[0].get(), nodes[1].get(), 0.3f
                        )
                    );
    
                    std::swap(nodes[0], nodes[1]);
                    
                    std::cout << ">>> Swapped Node 0 and Node 1!\n";
                }
            }
        }

        if (keyPressed->code == sf::Keyboard::Key::Backspace) {
            if (!edges.empty()) {
                UI::DSA::Edge* edgeToDeletePtr = edges.back().get();
                
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::EdgeDeleteAnimation>(
                        edgeToDeletePtr, 
                        0.3f, 
                        [this]() {
                            if (!edges.empty()) {
                                edges.pop_back();
                                std::cout << "[-] Edge Deleted successfully!\n";
                            }
                        }
                    )
                );
            }
            if (!nodes.empty()) {
                UI::DSA::Node* nodeToDeletePtr = nodes.back().get();
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::NodeDeleteAnimation>(
                        nodeToDeletePtr, 0.3f, 
                        [this]() { 
                            if (!nodes.empty()) {
                                nodes.pop_back();       
                                drawOrder.pop_back();   
                                std::cout << "[-] Node Deleted successfully!\n";
                            }
                        }
                    )
                );
            }
        }
        if (keyPressed->code == sf::Keyboard::Key::H) {
            if (!nodes.empty()) {
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::NodeHighlightAnimation>(
                        nodes[0].get(), .3f
                    )
                );
                std::cout << ">>> Node 0 Highlighted!\n";
            }

            if (!edges.empty()) {
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::EdgeHighlightAnimation>(edges[0].get(), 0.3f)
                );
                std::cout << ">>> Edge 0 Highlighted!\n";
            }
        }

        // Test Unhighlight (Phím U)
        if (keyPressed->code == sf::Keyboard::Key::U) {
            if (!nodes.empty()) {
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::NodeUnhighlightAnimation>(
                        nodes[0].get(), .3f
                    )
                );
                std::cout << ">>> Node 0 Unhighlighted!\n";
            }

            if (!edges.empty()) {
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::EdgeUnhighlightAnimation>(edges[0].get(), 0.3f)
                );
                std::cout << ">>> Edge 0 Highlighted!\n";
            }
        }
        if (keyPressed->code == sf::Keyboard::Key::P) {
            // Chỉ test khi có ít nhất 2 node và 1 cạnh
            if (nodes.size() >= 2 && !edges.empty()) {
                UI::DSA::Node* targetNode = nodes.back().get();
                UI::DSA::Edge* targetEdge = edges.back().get();

                // 1. Kịch bản tổng thể là một chuỗi (Nối tiếp)
                auto masterSequence = std::make_unique<UI::Animations::SequenceAnimation>();

                // 2. BƯỚC 1: BUNG LÊN & ĐỔI MÀU (Chạy Song song)
                auto step1 = std::make_unique<UI::Animations::ParallelAnimation>();
                step1->add(std::make_unique<UI::Animations::NodeHighlightAnimation>(targetNode, 0.3f));
                step1->add(std::make_unique<UI::Animations::EdgeHighlightAnimation>(targetEdge, 0.3f));
                step1->add(std::make_unique<UI::Animations::NodeScaleAnimation>(targetNode, 1.0f, 1.5f, 0.3f)); // Phóng to 1.5x
                
                masterSequence->add(std::move(step1)); // Nạp Bước 1 vào chuỗi chính

                // 3. BƯỚC 2: THU VỀ & TRẢ MÀU CŨ (Chạy Song song)
                auto step2 = std::make_unique<UI::Animations::ParallelAnimation>();
                step2->add(std::make_unique<UI::Animations::NodeUnhighlightAnimation>(targetNode, 0.3f));
                step2->add(std::make_unique<UI::Animations::EdgeUnhighlightAnimation>(targetEdge, 0.3f));
                step2->add(std::make_unique<UI::Animations::NodeScaleAnimation>(targetNode, 1.5f, 1.0f, 0.3f)); // Thu về 1.0x
                
                masterSequence->add(std::move(step2)); // Nạp Bước 2 vào chuỗi chính

                // Đưa cho Manager chạy toàn bộ kịch bản
                ctx.animManager.addAnimation(std::move(masterSequence));
                
                std::cout << ">>> Playing Nested Parallel Animations!\n";
            }
        }
    }

    //event for drawing nodes
    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);
            for (int i = drawOrder.size() - 1; i >= 0; --i) {
                int nodeIdx = drawOrder[i]; 
                if (nodes[nodeIdx]->contains(mousePos)) {
                    draggedNodeIndex = nodeIdx;
                    dragOffset = nodes[nodeIdx]->getPosition() - mousePos;
                    drawOrder.erase(drawOrder.begin() + i);
                    drawOrder.push_back(nodeIdx);
                    break; 
                }
            }
        }
    }

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) draggedNodeIndex = -1;
    }
}

void TestScreen::update() {
    sf::Vector2i mousePosi = sf::Mouse::getPosition(ctx.window);
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(mousePosi);
    
    btnInsert.update(mousePosi);
    
    // Update Node positions (dragging)
    if (draggedNodeIndex != -1) {
        nodes[draggedNodeIndex]->setPosition(mousePos + dragOffset);
    } 
    else {
        // ... (Keep your existing hover logic)
        int hoveredNodeIdx = -1;
        for (int i = drawOrder.size() - 1; i >= 0; --i) {
            int realIdx = drawOrder[i];
            if (nodes[realIdx]->contains(mousePos)) {
                hoveredNodeIdx = realIdx; 
                break; 
            }
        }
        for (int i = 0; i < nodes.size(); ++i) {
            if (i == hoveredNodeIdx) {
                nodes[i]->onHover(); 
            } else {
                nodes[i]->onIdle(); 
            }
        }
    }

    // Update all edges by giving them access to the node list
    for (auto& edge : edges) {
        edge->update();
    }
}

void TestScreen::draw() {
    // 4. Draw order: Edges FIRST (so they appear under nodes)
    for (auto& edge : edges) {
        edge->draw();
    }

    // Then draw nodes
    for (int idx : drawOrder) {
        nodes[idx]->draw();
    }

    btnInsert.draw(); // UI on top
}