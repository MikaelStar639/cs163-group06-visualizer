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

    if (uiMenu.consumeGoClicked()) {
        if (myGraph.isAnimating()) {
            std::cout << "[WARNING] Wait!\n";
            return;
        }
        
        handleMenuAction();
        uiMenu.clearInputs();
        
        // Clean/Clear action is index 4 in getMainButtonLabels
        if (uiMenu.getActiveMenuIndex() == 4) {
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
    int menuIndex = uiMenu.getActiveMenuIndex();
    if (menuIndex == -1) return;
    
    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    // Guard: Prevent heap operations if data isn't heapified (Indices 1, 2, 3)
    if (isRawData && (menuIndex == 1 || menuIndex == 2 || menuIndex == 3)) {
        std::cout << "[UI LOG] Please Heapify the data first!\n";
        return;
    }

    if (menuIndex == 0) { // Create
        if (sel == 0) { // Random
            int subBtn = uiMenu.getClickedSubButtonIndex();
            if (subBtn == 0) { // Create (Raw)
                std::string sizeStr = !inputs.empty() ? inputs[0].getText() : "";
                if (sizeStr.empty()) return;
                controller.handleCreateRandom(std::stoi(sizeStr));
                
                isRawData = true;
                uiMenu.setMainButtonEnabled(1, false);
                uiMenu.setMainButtonEnabled(2, false);
                uiMenu.setMainButtonEnabled(3, false);
            } 
            else if (subBtn == 1) { // Heapify
                controller.handleBuildHeap(model.getPool());
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
            else if (subBtn == 2) { // Create Pre-Heapified (NEW)
                std::string sizeStr = !inputs.empty() ? inputs[0].getText() : "";
                if (sizeStr.empty()) return;
                
                // Call the new instant function
                controller.handlePreHeapifiedRandom(std::stoi(sizeStr));
                
                // It's already heapified, so we set raw to false and enable everything
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
        } else if (sel == 1) { // File selection
            int subBtn = uiMenu.getClickedSubButtonIndex();
            
            if (subBtn == 0) { // Edit
                controller.handleEditDataFile();
            } 
            else if (subBtn == 1) { // Create (Raw)
                controller.handleCreateFromFile();
                isRawData = true;
                uiMenu.setMainButtonEnabled(1, false);
                uiMenu.setMainButtonEnabled(2, false);
                uiMenu.setMainButtonEnabled(3, false);
            } 
            else if (subBtn == 2) { // Heapify
                controller.handleBuildHeap(model.getPool());
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
            else if (subBtn == 3) { // Create Pre-Heapified (NEW)
                controller.handlePreHeapifiedFromFile();
                
                // Data is ready for use
                isRawData = false;
                uiMenu.setMainButtonEnabled(1, true);
                uiMenu.setMainButtonEnabled(2, true);
                uiMenu.setMainButtonEnabled(3, true);
            }
        }
    }
    else if (menuIndex == 1) { // Insert
        if (inputs.empty() || inputs[0].getText().empty()) return;
        controller.handleInsert(std::stoi(inputs[0].getText()));
    }
    else if (menuIndex == 2) { // Delete Root
        controller.handleRemoveRoot();
    }
    else if (menuIndex == 3) { // Find Maximum
        controller.handleReturnRoot();
    }
    else if (menuIndex == 4) { // Clear All
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
        controller.forceVisualSync(); 
        codeViewer.hide();
    }
    
    DSAScreenBase::update();
}

void HeapScreen::draw() {
    DSAScreenBase::draw(); // set camera & draw the graph
    codeViewer.draw(ctx.window); // Pseudo-code panel (screen-space)
    uiMenu.draw(ctx.window); // UI menu on top
}