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
      codeViewer(context.font),
      controller(context, myGraph, model, &codeViewer),
      isRawData(false)
{
    myGraph.setDraggable(false);
    
    // Position pseudo-code panel at bottom-right
    sf::Vector2u winSize = context.window.getSize();
    codeViewer.setPositionBottomRight(static_cast<float>(winSize.x), static_cast<float>(winSize.y), 50.f);
}

void HeapScreen::handleEvent(const sf::Event& event) {
    uiMenu.handleEvent(event);
    DSAScreenBase::handleEvent(event);
    
    if (uiMenu.isBackClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    // Capture specific sub-button clicks (mapped in DSAMenuBase)
    int clickedSubBtn = uiMenu.getClickedSubButtonIndex();

    if (clickedSubBtn != -1 || uiMenu.consumeGoClicked()) {
        if (myGraph.isAnimating()) {
            std::cout << "[WARNING] Wait!\n";
            return;
        }
        
        // Use the index for sub-button logic (Create/Heapify/Edit)
        handleMenuAction(uiMenu.consumeClickedSubButtonIndex());
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

void HeapScreen::handleMenuAction(int subBtnIndex) {
    using namespace UI::Widgets;
    ActiveMenu menu = uiMenu.getActiveMenu();
    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    // Guard: Prevent interaction if data isn't heapified (unless creating or cleaning)
    if (isRawData && menu != ActiveMenu::Create && menu != ActiveMenu::Clean) {
        std::cout << "[UI LOG] Please Heapify the data first!\n";
        return;
    }

    if (menu == ActiveMenu::Create) {
        if (sel == 0) { // Random
            if (subBtnIndex == 0) { // "Create" button
                std::string sizeStr = !inputs.empty() ? inputs[0].getText() : "";
                if (sizeStr.empty()) return;
                controller.handleCreateRandom(std::stoi(sizeStr));
                isRawData = true;
                uiMenu.setMainButtonEnabled(1, false); // Grey out Insert
                uiMenu.setMainButtonEnabled(2, false); // Grey out Delete Root
                uiMenu.setMainButtonEnabled(3, false); // Grey out Find Max
            } 
            else if (subBtnIndex == 1) { // "Heapify" button
                controller.handleBuildHeap(model.getPool());
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
        } else if (sel == 1) { // File
            if (subBtnIndex == 0) {
                controller.handleEditDataFile();
            } else if (subBtnIndex == 1) { // "Create" button
                controller.handleCreateFromFile();
                isRawData = true;
                uiMenu.setMainButtonEnabled(1, false);
                uiMenu.setMainButtonEnabled(2, false);
                uiMenu.setMainButtonEnabled(3, false);
            } else if (subBtnIndex == 2) { // "Heapify" button
                controller.handleBuildHeap(model.getPool());
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
        }
    }
    else if (menu == ActiveMenu::Insert) {
        if (inputs.empty() || inputs[0].getText().empty()) return;
        controller.handleInsert(std::stoi(inputs[0].getText()));
    }
    else if (menu == ActiveMenu::Remove) { // Delete Root
        controller.handleRemoveRoot();
    }
    else if (menu == ActiveMenu::Search) { // Find Maximum
        controller.handleReturnRoot();
    }
    else if (menu == ActiveMenu::Update) {
        controller.handleClearAll();
        isRawData = false;
        uiMenu.setMainButtonEnabled(1, true);
        uiMenu.setMainButtonEnabled(2, true);
        uiMenu.setMainButtonEnabled(3, true);
    }
}

void HeapScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos);

    if (uiMenu.consumeCancelClicked()) {
        ctx.animManager.clearAll();
        myGraph.resetVisuals();
        controller.forceSnapLayout(); 
        codeViewer.hide();
    }

    DSAScreenBase::update();
}

void HeapScreen::draw() {
    DSAScreenBase::draw();       // set camera & draw the graph
    codeViewer.draw(ctx.window); // Pseudo-code panel (screen-space)
    uiMenu.draw(ctx.window);     // UI menu on top
}