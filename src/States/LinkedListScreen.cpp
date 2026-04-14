#include "States/LinkedListScreen.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include <iostream>


LinkedListScreen::LinkedListScreen(AppContext& context)
    : DSAScreenBase(context), // parent constructor
      uiMenu(context),
      codeViewer(context.font),
      controller(context, myGraph, model, &codeViewer)
{
    myGraph.setDraggable(false);
    
    // Position pseudo-code panel at bottom-right
    sf::Vector2u winSize = context.window.getSize();
    codeViewer.setPositionBottomRight(static_cast<float>(winSize.x), static_cast<float>(winSize.y), 50.f);
}

void LinkedListScreen::handleEvent(const sf::Event& event) {
    uiMenu.handleEvent(event);
    DSAScreenBase::handleEvent(event);
    
    if (uiMenu.isBackClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    if (uiMenu.consumeGoClicked()) {
        if (myGraph.isAnimating()) {
            std::cout << "[WARNING] Wait!\n";
            return;
        }
        
        handleMenuAction();
        uiMenu.clearInputs();

        if (uiMenu.getActiveMenu() == UI::Widgets::ActiveMenu::Clean) {
            uiMenu.resetMenu();
        }
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape){
            ctx.nextState = ScreenState::MainMenu;
        }
    }
}

void LinkedListScreen::handleMenuAction() {
    using namespace UI::Widgets;
    ActiveMenu menu = uiMenu.getActiveMenu();
    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    if (menu == ActiveMenu::Create) {
        if (sel == 0) { // Random
            std::string sizeStr = !inputs.empty() ? inputs[0].getText() : "";
            if (sizeStr.empty()) return;
            int size = std::stoi(sizeStr);
            controller.handleCreateRandom(size);
        } else if (sel == 1) { // File
            int subBtn = uiMenu.getClickedSubButtonIndex();
            if (subBtn == 0) controller.handleEditDataFile();
            else if (subBtn == 1) controller.handleCreateFromFile();
        }
    }
    else if (menu == ActiveMenu::Insert) {
        int val = 0;
        int pos = 0;

        if (sel == 2) { 
            if (inputs.size() < 2 || inputs[0].getText().empty() || inputs[1].getText().empty()) return;
            
            pos = std::stoi(inputs[0].getText());
            val = std::stoi(inputs[1].getText());
            
        } else { 
            if (inputs.empty() || inputs[0].getText().empty()) return;
            
            val = std::stoi(inputs[0].getText());
        }
        
        controller.handleInsert(sel, pos, val);
    }
    else if (menu == ActiveMenu::Remove) {
        int pos = (sel == 2 && !inputs.empty()) ? std::stoi(inputs[0].getText()) : 0;
        controller.handleRemove(sel, pos);
    }
    else if (menu == ActiveMenu::Search) {
        std::string valStr = !inputs.empty() ? inputs[0].getText() : "";
        if (valStr.empty()) return;
        
        controller.handleSearch(std::stoi(valStr));
    }
    else if (menu == ActiveMenu::Update) {
        int arg1 = (inputs.size() > 0 && !inputs[0].getText().empty()) ? std::stoi(inputs[0].getText()) : 0;
        int arg2 = (inputs.size() > 1 && !inputs[1].getText().empty()) ? std::stoi(inputs[1].getText()) : 0;
        
        controller.handleUpdate(sel, arg1, arg1, arg2);
    }
    else if (menu == ActiveMenu::Clean) {
        controller.handleClearAll();
    }
}

void LinkedListScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos);
    DSAScreenBase::update();
}

void LinkedListScreen::draw() {
    DSAScreenBase::draw(); // set camera & draw the graph
    codeViewer.draw(ctx.window); // Pseudo-code panel (screen-space)
    uiMenu.draw(ctx.window); // UI menu on top
}