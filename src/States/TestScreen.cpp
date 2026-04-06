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
      btnInsert(context, "INSERT NODE", {50.f, 50.f}, {250.f, 60.f}),
      myGraph(ctx, true)
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
    myGraph.addNode(val, sf::Vector2f(finalX, finalY));

    if (myGraph.getNodes().size() >= 2) {
        myGraph.addEdge(myGraph.getNodes().size() - 2, myGraph.getNodes().size() - 1, "");
    }
}

void TestScreen::handleEvent(const sf::Event& event) {
    sf::Vector2i mousePosi = sf::Mouse::getPosition(ctx.window);
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(mousePosi);

    if (btnInsert.isClicked(event)) addNewNode(std::to_string(std::rand() % 100));
    
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Backspace) {
            myGraph.removeLastNode();
        }
    }

    myGraph.handleEvent(event, mousePos); 
}

void TestScreen::update() {
    sf::Vector2i mousePosi = sf::Mouse::getPosition(ctx.window);
    sf::Vector2f mousePos = static_cast<sf::Vector2f>(mousePosi);
    
    btnInsert.update(mousePosi);
    
    myGraph.update();
}

void TestScreen::draw() {
    myGraph.draw();
    btnInsert.draw(); 
}