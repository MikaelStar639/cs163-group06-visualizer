#include "States/HeapScreen.hpp"
#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Core/SequenceAnimation.hpp"
#include "UI/Animations/Core/CallbackAnimation.hpp"
#include <iostream>

HeapScreen::HeapScreen(AppContext& context)
    : DSAScreenBase(context),
      uiMenu(context),
      controller(context, myGraph, model)
{
    myGraph.setDraggable(false);
}

void HeapScreen::handleEvent(const sf::Event& event) {
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

void HeapScreen::handleMenuAction() {
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
        } 
        else if (sel == 1) { // File
            int subBtn = uiMenu.getClickedSubButtonIndex();
            if (subBtn == 0) controller.handleEditDataFile();
            else if (subBtn == 1) controller.handleCreateFromFile();
        }
    }
    else if (menu == ActiveMenu::Insert) {
        std::string valStr = !inputs.empty() ? inputs[0].getText() : "";
        if (valStr.empty()) return;

        controller.handleInsert(std::stoi(valStr));
    }
    else if (menu == ActiveMenu::Remove) {
        std::string valStr = !inputs.empty() ? inputs[0].getText() : "";
        if (valStr.empty()) return;

        controller.handleRemove(std::stoi(valStr));
    }
    else if (menu == ActiveMenu::Search) {
        std::string valStr = !inputs.empty() ? inputs[0].getText() : "";
        if (valStr.empty()) return;

        controller.handleSearch(std::stoi(valStr));
    }
    else if (menu == ActiveMenu::Update) {
        if (inputs.size() < 2 || inputs[0].getText().empty() || inputs[1].getText().empty()) return;

        int oldVal = std::stoi(inputs[0].getText());
        int newVal = std::stoi(inputs[1].getText());

        controller.handleUpdate(oldVal, newVal);
    }
    else if (menu == ActiveMenu::Clean) {
        controller.handleClearAll();
    }
}

void HeapScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos);

    controller.processDeferredActions();

    DSAScreenBase::update();
}

void HeapScreen::draw() {
    DSAScreenBase::draw();
    uiMenu.draw(ctx.window);
}