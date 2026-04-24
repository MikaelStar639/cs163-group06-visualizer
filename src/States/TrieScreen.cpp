#include "States/TrieScreen.hpp"
#include <iostream>

TrieScreen::TrieScreen(AppContext& context)
    : DSAScreenBase(context), 
      uiMenu(context),
      codeViewer(context.font),
      controller(context, myGraph, model, &codeViewer) 
{
    myGraph.setDraggable(false);
    sf::Vector2u winSize = context.window.getSize();
    codeViewer.setPositionBottomRight(static_cast<float>(winSize.x), static_cast<float>(winSize.y), 50.f);
}

void TrieScreen::handleEvent(const sf::Event& event) {
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

        if (uiMenu.getActiveMenuIndex() == static_cast<int>(UI::Widgets::TrieMenu::Action::Clean)) {
            uiMenu.resetMenu();
        }
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape){
            ctx.nextState = ScreenState::MainMenu;
        }
    }
}

void TrieScreen::handleMenuAction() {
    using namespace UI::Widgets;
    
    int menuIndex = uiMenu.getActiveMenuIndex();
    if (menuIndex == -1) return;

    TrieMenu::Action menu = static_cast<TrieMenu::Action>(menuIndex);
    
    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    std::string word = !inputs.empty() ? inputs[0].getText() : "";

    if (menu == TrieMenu::Action::Create) {
        if (sel == 0) { // Random
            std::string sizeStr = !inputs.empty() ? inputs[0].getText() : "";
            if (sizeStr.empty()) return;
            int count = std::stoi(sizeStr);
            controller.handleCreateRandom(count);
        } else if (sel == 1) { // File
            int subBtn = uiMenu.getClickedSubButtonIndex();
            if (subBtn == 0) controller.handleEditDataFile();
            else if (subBtn == 1) controller.handleCreateFromFile();
        }
    }
    else if (menu == TrieMenu::Action::Insert) {
        if (!word.empty()) controller.handleInsert(word);
    }
    else if (menu == TrieMenu::Action::Search) {
        if (!word.empty()) {
            bool isPrefix = (sel == 1); 
            controller.handleSearch(word, isPrefix); 
        }
    }
    else if (menu == TrieMenu::Action::Remove) {
        if (!word.empty()) controller.handleRemove(word);
    }
    else if (menu == TrieMenu::Action::Clean) {
        controller.handleClearAll();
    }
}

void TrieScreen::update(float dt) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos, dt);

    if (uiMenu.consumeCancelClicked()) {
        ctx.animManager.clearAll();
        myGraph.resetVisuals();
        controller.forceSnapLayout(); 
        codeViewer.hide();
    }

    if (uiMenu.consumeSkipClicked()) {
        ctx.animManager.clearAll();     
        myGraph.resetVisuals();         
        controller.forceSnapLayout();   
        codeViewer.hide();
    }

    DSAScreenBase::update(dt);
}

void TrieScreen::draw() {
    DSAScreenBase::draw();       
    uiMenu.draw(ctx.window);     
    codeViewer.draw(ctx.window); 
}