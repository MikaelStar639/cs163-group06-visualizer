#include "States/TestScreen.hpp"
#include "Core/Constants.hpp"
#include "UI/Animations/ScaleAnimation.hpp"
#include "UI/Animations/InsertAnimation.hpp"
#include "UI/Animations/DeleteAnimation.hpp"
#include "UI/Animations/SwapAnimation.hpp"
#include <iostream>

#include <cstdlib> 
#include <ctime>   

TestScreen::TestScreen(AppContext& context)
    : ctx(context), 
      btnInsert(context, "INSERT NODE", {50.f, 50.f}, {250.f, 60.f}) 
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}


//Add a new Node in a random position
void TestScreen::addNewNode(const std::string &val){

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

    nodes.emplace_back(std::make_unique<UI::DSA::Node>(ctx, val, sf::Vector2f(finalX, finalY)));
    drawOrder.push_back(nodes.size() - 1);

    UI::DSA::Node* newNodePtr = nodes.back().get();
    ctx.animManager.addAnimation(
        std::make_unique<UI::Animations::InsertAnimation>(newNodePtr, 0.3f)
    );


    std::cout << ">>> Node '" << val << "' inserted successfully at (" << finalX << ", " << finalY << ")\n";
}

void TestScreen::handleEvent(const sf::Event& event) {
    
    //click the button to add a random value
    if (btnInsert.isClicked(event)) addNewNode(std::to_string(std::rand() % 100));
    
    //press enter to input the value
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
                        std::make_unique<UI::Animations::SwapAnimation>(
                            nodes[0].get(), nodes[1].get(), 0.3f
                        )
                    );
    
                    std::swap(nodes[0], nodes[1]);
                    
                    std::cout << ">>> Swapped Node 0 and Node 1!\n";
                }
            }
        }

        if (keyPressed->code == sf::Keyboard::Key::Backspace) {
            if (!nodes.empty()) {
                UI::DSA::Node* nodeToDeletePtr = nodes.back().get();
                ctx.animManager.addAnimation(
                    std::make_unique<UI::Animations::DeleteAnimation>(
                        nodeToDeletePtr, 0.3f, 
                        [this]() { 
                            if (!nodes.empty()) {
                                nodes.pop_back();       // Xóa data thật khỏi bộ nhớ
                                drawOrder.pop_back();   // Cập nhật lại mảng vẽ
                                std::cout << "[-] Node Deleted successfully!\n";
                            }
                        }
                    )
                );
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
    
    if (draggedNodeIndex != -1) {
        nodes[draggedNodeIndex]->setPosition(mousePos + dragOffset);
    } 
    else {
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
}

void TestScreen::draw() {
    btnInsert.draw();
    for (int idx : drawOrder) {
        nodes[idx]->draw();
    }
}