#include "States/LinkedListScreen.hpp"
#include <iostream>

LinkedListScreen::LinkedListScreen(AppContext& context)
    : ctx(context), 
      uiMenu(context),
      myGraph(context, true)
{
}

void LinkedListScreen::handleEvent(const sf::Event& event) {
    uiMenu.handleEvent(event);
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    myGraph.handleEvent(event, static_cast<sf::Vector2f>(mousePos));

    if (uiMenu.isBackClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    if (uiMenu.consumeGoClicked()) {
        handleMenuAction();
        uiMenu.clearInputs();
        uiMenu.resetMenu();
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) ctx.nextState = ScreenState::MainMenu;
    }
}

void LinkedListScreen::handleMenuAction() {
    using namespace UI::Widgets;
    ActiveMenu menu = uiMenu.getActiveMenu();
    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    if (menu == ActiveMenu::Create) {
        if (sel == 0) {
            std::string size = !inputs.empty() ? inputs[0].getText() : "";
            std::cout << "[UI LOG] Create Random | Size = " << size << std::endl;
        } else if (sel == 1) {
            std::string path = !inputs.empty() ? inputs[0].getText() : "";
            std::cout << "[UI LOG] Create File | Path = " << path << std::endl;
        }
    }
    else if (menu == ActiveMenu::Insert) {
        std::string val = !inputs.empty() ? inputs[0].getText() : "";
        if (sel == 0) std::cout << "[UI LOG] Insert Head | Val = " << val << std::endl;
        else if (sel == 1) std::cout << "[UI LOG] Insert Tail | Val = " << val << std::endl;
        else if (sel == 2) {
            std::string pos = inputs.size() > 1 ? inputs[1].getText() : "";
            std::cout << "[UI LOG] Insert At | Val = " << val << " | Pos = " << pos << std::endl;
        }
    }
    else if (menu == ActiveMenu::Remove) {
        if (sel == 0) std::cout << "[UI LOG] Delete Head" << std::endl;
        else if (sel == 1) std::cout << "[UI LOG] Delete Tail" << std::endl;
        else if (sel == 2) {
            std::string pos = !inputs.empty() ? inputs[0].getText() : "";
            std::cout << "[UI LOG] Delete At | Pos = " << pos << std::endl;
        }
    }
    else if (menu == ActiveMenu::Search) {
        std::string val = !inputs.empty() ? inputs[0].getText() : "";
        std::cout << "[UI LOG] Search | Val = " << val << std::endl;
    }
    else if (menu == ActiveMenu::Update) {
        if (sel == 0) {
            std::string pos = inputs.size() > 0 ? inputs[0].getText() : "";
            std::string newVal = inputs.size() > 1 ? inputs[1].getText() : "";
            std::cout << "[UI LOG] Update At | Pos = " << pos << " | New Val = " << newVal << std::endl;
        } else if (sel == 1) {
            std::string oldVal = inputs.size() > 0 ? inputs[0].getText() : "";
            std::string newVal = inputs.size() > 1 ? inputs[1].getText() : "";
            std::cout << "[UI LOG] Update By Value | Old Val = " << oldVal << " | New Val = " << newVal << std::endl;
        }
    }
    else if (menu == ActiveMenu::Clean) {
        // In the base class, Clean triggers goClicked immediately
        std::cout << "[UI LOG] Action executed: Clear All" << std::endl;
    }
}

void LinkedListScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos);
    myGraph.update();
}

void LinkedListScreen::draw() {
    myGraph.draw();
    uiMenu.draw(ctx.window);
}
