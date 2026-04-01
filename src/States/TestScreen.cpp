#include "States/TestScreen.hpp"
#include "Core/Constants.hpp"
#include <iostream>
#include <cstdlib> 
#include <ctime>   

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
    nodes.emplace_back(ctx, val, sf::Vector2f(finalX, finalY));
    UI::DSA::Node* newNodePtr = &nodes.back();

    size_t newNodeIndex = nodes.size() - 1;

    // Connect all existing nodes to the newest one using their index
    for (size_t i = 0; i < newNodeIndex; ++i) {
        edges.emplace_back(i, newNodeIndex, ctx); 
        
        sf::Color randomColor(std::rand() % 256, std::rand() % 256, std::rand() % 256);
        edges.back().setColor(randomColor);
        edges.back().toggleDirection(1);
        edges.back().flipDirection();
        edges.back().setThickness(3);
    }
    
    drawOrder.push_back(newNodeIndex);
    std::cout << ">>> Node '" << val << "' inserted and connected to " << nodes.size() - 1 << " nodes.\n";
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

    if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mouseEvent->button == sf::Mouse::Button::Left) {
            sf::Vector2f mousePos(mouseEvent->position.x, mouseEvent->position.y);
            for (int i = drawOrder.size() - 1; i >= 0; --i) {
                int nodeIdx = drawOrder[i]; 
                if (nodes[nodeIdx].contains(mousePos)) {
                    draggedNodeIndex = nodeIdx;
                    dragOffset = nodes[nodeIdx].getPosition() - mousePos;
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
        nodes[draggedNodeIndex].setPosition(mousePos + dragOffset);
    } 
    else {
        // ... (Keep your existing hover logic)
        int hoveredNodeIdx = -1;
        for (int i = drawOrder.size() - 1; i >= 0; --i) {
            int realIdx = drawOrder[i];
            if (nodes[realIdx].contains(mousePos)) {
                hoveredNodeIdx = realIdx; 
                break; 
            }
        }
        for (int i = 0; i < nodes.size(); ++i) {
            if (i == hoveredNodeIdx) nodes[i].onHover(); 
            else nodes[i].onIdle(); 
        }
    }

    // Update all edges by giving them access to the node list
    for (auto& edge : edges) {
        edge.update(nodes);
    }
}

void TestScreen::draw() {
    // 4. Draw order: Edges FIRST (so they appear under nodes)
    for (auto& edge : edges) {
        edge.draw(ctx.window);
    }

    // Then draw nodes
    for (int idx : drawOrder) {
        nodes[idx].draw();
    }

    btnInsert.draw(); // UI on top
}