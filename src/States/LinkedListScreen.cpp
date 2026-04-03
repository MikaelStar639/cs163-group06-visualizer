#include "States/LinkedListScreen.hpp"
#include "Core/Constants.hpp"
#include <iostream>

LinkedListScreen::LinkedListScreen(AppContext& context)
    : ctx(context),
      btnBack(context, " MENU ", {0.f, 0.f}, {120.f, 50.f}),
      
      btnCreate(context, "Create", {0.f, 0.f}, {160.f, 55.f}),
      btnInsert(context, "Insert", {0.f, 0.f}, {160.f, 55.f}),
      btnRemove(context, "Delete", {0.f, 0.f}, {160.f, 55.f}),
      btnSearch(context, "Search", {0.f, 0.f}, {160.f, 55.f}),
      btnClean(context, "Clear All", {0.f, 0.f}, {160.f, 55.f}),

      panelBg({300.f, 150.f}, Config::UI::BUTTON_CORNER_RADIUS),
      lblParam1(context.font, " ", 18),
      lblParam2(context.font, " ", 18),

      inputParam1(context, {0.f, 0.f}, {100.f, 40.f}, "", UI::Widgets::InputType::Integer),
      inputParam2(context, {0.f, 0.f}, {100.f, 40.f}, "", UI::Widgets::InputType::Integer),

      dropdownAction(context, "Select...", {0.f, 0.f}, {120.f, 40.f}),
      btnExecute(context, "Go", {0.f, 0.f}, {80.f, 40.f}),

      btnPrev(context, "|<", {700.f, 840.f}, {60.f, 40.f}),
      btnPlay(context, "||", {770.f, 840.f}, {60.f, 40.f}),
      btnNext(context, ">|", {840.f, 840.f}, {60.f, 40.f}),
      
      title(context.font, "Linked List", 24)
{
    mainButtons = { &btnCreate, &btnInsert, &btnRemove, &btnSearch, &btnClean };
    initUI();
    updateLayout();
}

void LinkedListScreen::initUI() {
    title.setFillColor(Config::UI::Colors::ButtonHover);
    title.setStyle(sf::Text::Bold);
    title.setOrigin({0.f, 0.f});
    title.setPosition({140.f, 10.f}); 

    auto applyBtnColors = [](UI::Widgets::Button& b) {
        b.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, 
                    Config::UI::Colors::ButtonPressed, Config::UI::Colors::ButtonText);
    };

    applyBtnColors(btnBack); 
    applyBtnColors(btnPrev); applyBtnColors(btnPlay); applyBtnColors(btnNext);
    
    sf::Color innerBtnIdle(40, 60, 60);
    sf::Color innerBtnHover(60, 80, 80);
    sf::Color innerBtnPress(30, 50, 50);
    
    btnExecute.setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
    dropdownAction.setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);

    lblParam1.setFillColor(sf::Color::White);
    lblParam2.setFillColor(sf::Color::White);

    sf::Color panelColor(122, 160, 142);
    panelBg.setFillColor(panelColor);
    panelBg.setOutlineThickness(2.f);
    panelBg.setOutlineColor(sf::Color(200, 220, 200));
}

void LinkedListScreen::renderSubMenu(float boxY, ActiveMenu type) {
    float mainX = 30.f;  
    float boxX = mainX + 160.f + 10.f; 
    float innerX = boxX + 15.f;
    float innerY = boxY + 15.f;
    float boxWidth = 260.f;
    float boxHeight = 100.f;

    activeSubButtons.clear();
    activeInputs.clear();
    activeLabels.clear(); // We no longer use external text labels!
    
    dropdownAction.setSize({180.f, 40.f});
    btnExecute.setSize({100.f, 40.f});

    if (type == ActiveMenu::Create) {
        dropdownAction.setOptions({"Random", "File"});
        if (dropdownAction.getSelectedIndex() == -1) dropdownAction.setSelectedIndex(0);
        dropdownAction.setPosition({innerX, innerY});
        
        int sel = dropdownAction.getSelectedIndex();
        float nextY = innerY + 40.f + 15.f;
        boxWidth = 220.f; 
        boxHeight = nextY + 15.f - boxY;

        if (sel != -1) {
            if (sel == 0) { // Random
                inputParam1.setPlaceholder("Size");
                inputParam1.setPosition({innerX, nextY});
                inputParam1.setSize({160.f, 40.f});
                activeInputs.push_back(&inputParam1);
                nextY += 40.f + 15.f;
            }
            btnExecute.setPosition({innerX, nextY});
            btnExecute.setLabel("Go");
            activeSubButtons.push_back(&btnExecute);
            
            boxHeight = nextY + 40.f + 15.f - boxY;
        }
    }
    else if (type == ActiveMenu::Insert) {
        dropdownAction.setOptions({"Head", "Tail", "At"});
        if (dropdownAction.getSelectedIndex() == -1) dropdownAction.setSelectedIndex(0);
        dropdownAction.setPosition({innerX, innerY});
        
        int sel = dropdownAction.getSelectedIndex();
        float nextY = innerY + 40.f + 15.f;
        boxWidth = 220.f; 
        boxHeight = nextY + 15.f - boxY;

        if (sel != -1) {
            inputParam1.setPlaceholder("Value");
            inputParam1.setPosition({innerX, nextY});
            inputParam1.setSize({160.f, 40.f});
            activeInputs.push_back(&inputParam1);
            nextY += 40.f + 10.f;

            if (sel == 2) { // 'At'
                inputParam2.setPlaceholder("Position");
                inputParam2.setPosition({innerX, nextY});
                inputParam2.setSize({160.f, 40.f});
                activeInputs.push_back(&inputParam2);
                nextY += 40.f + 10.f;
            }

            btnExecute.setPosition({innerX, nextY + 5.f});
            btnExecute.setLabel("Go");
            activeSubButtons.push_back(&btnExecute);
            
            boxHeight = nextY + 40.f + 20.f - boxY;
        }
    }
    else if (type == ActiveMenu::Remove) {
        dropdownAction.setOptions({"Head", "Tail", "At"});
        if (dropdownAction.getSelectedIndex() == -1) dropdownAction.setSelectedIndex(0);
        dropdownAction.setPosition({innerX, innerY});
        
        int sel = dropdownAction.getSelectedIndex();
        float nextY = innerY + 40.f + 15.f;
        boxWidth = 220.f; 
        boxHeight = nextY + 15.f - boxY;

        if (sel != -1) {
            if (sel == 2) { // 'At'
                inputParam2.setPlaceholder("Position");
                inputParam2.setPosition({innerX, nextY});
                inputParam2.setSize({160.f, 40.f});
                activeInputs.push_back(&inputParam2);
                nextY += 40.f + 15.f;
            }

            btnExecute.setPosition({innerX, nextY});
            btnExecute.setLabel("Go");
            activeSubButtons.push_back(&btnExecute);
            
            boxHeight = nextY + 40.f + 15.f - boxY;
        }
    }
    else if (type == ActiveMenu::Search) {
        inputParam1.setPlaceholder("Value");
        inputParam1.setPosition({innerX, innerY});
        inputParam1.setSize({160.f, 40.f});
        activeInputs.push_back(&inputParam1);
        
        float nextY = innerY + 40.f + 15.f;
        btnExecute.setPosition({innerX, nextY});
        btnExecute.setLabel("Go");
        
        boxWidth = 200.f; 
        boxHeight = nextY + 40.f + 15.f - boxY;
        activeSubButtons.push_back(&btnExecute);
    }
    
    if (dropdownAction.getIsDropped()) {
        float requiredDropHeight = (innerY - boxY) + 40.f + (int)3 * 40.f + 15.f; 
        if (boxHeight < requiredDropHeight) {
            boxHeight = requiredDropHeight;
        }
    }

    panelBg.setPosition({boxX, boxY});
    panelBg.setSize({boxWidth, boxHeight});
}

void LinkedListScreen::updateLayout() {
    float mainX = 30.f;      
    float gapMain = 5.f;    

    // --- Place Main Left Buttons ---
    ActiveMenu enums[] = {ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, ActiveMenu::Search, ActiveMenu::Clean};
    
    for (size_t i = 0; i < mainButtons.size(); ++i) {
        auto* b = mainButtons[i];
        bool isActive = (activeMenu == enums[i]);
        b->setSize({160.f, 55.f});
        b->setPosition({mainX, 150.f + (55.f + gapMain) * i});
        
        if (isActive) {
            b->setColors(sf::Color(122, 160, 142), sf::Color(122, 160, 142), sf::Color(122, 160, 142), sf::Color::White);
        } else {
            b->setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, Config::UI::Colors::ButtonPressed, Config::UI::Colors::ButtonText);
        }
    }

    activeSubButtons.clear();
    activeInputs.clear();
    activeLabels.clear();

    if (activeMenu == ActiveMenu::None || activeMenu == ActiveMenu::Clean) return;

    // --- Panel Settings ---
    float boxY = 150.f;
    for (size_t i = 0; i < 4; ++i) {
        if (activeMenu == enums[i]) {
            boxY = 150.f + (55.f + gapMain) * i;
            break;
        }
    }

    renderSubMenu(boxY, activeMenu);
}

void LinkedListScreen::handleEvent(const sf::Event& event) {
    if (btnBack.isClicked(event)) ctx.nextState = ScreenState::MainMenu;

    ActiveMenu enums[] = {ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, ActiveMenu::Search, ActiveMenu::Clean};
    for (size_t i = 0; i < mainButtons.size(); ++i) {
        if (mainButtons[i]->isClicked(event)) {
            activeMenu = (activeMenu == enums[i]) ? ActiveMenu::None : enums[i];
            
            inputParam1.clear();
            inputParam2.clear();
            dropdownAction.clearSelection();
            lastDropdownIndex = 0;
            
            if (activeMenu == ActiveMenu::Clean) {
                std::cout << "[UI LOG] Action executed: Clear All" << std::endl;
                activeMenu = ActiveMenu::None; 
            }
            updateLayout();
        }
    }

    if (activeMenu == ActiveMenu::Create || activeMenu == ActiveMenu::Insert || activeMenu == ActiveMenu::Remove) {
        if (dropdownAction.isClicked(event)) {
            if (dropdownAction.getSelectedIndex() != lastDropdownIndex) {
                 dropdownAction.setLabel(dropdownAction.getSelectedText());
                 lastDropdownIndex = dropdownAction.getSelectedIndex();
                 updateLayout();
            }
        }
    }

    // Pass event to active inputs, but only if dropdown is not active intercepting
    if (!dropdownAction.getIsDropped()) {
        for (auto* input : activeInputs) {
            input->handleEvent(event);
        }
    }

    if (btnExecute.isClicked(event)) {
        if (activeMenu == ActiveMenu::Create) {
             int sel = dropdownAction.getSelectedIndex();
             if (sel == 0) std::cout << "[UI LOG] Create Random | Size = " << inputParam1.getText() << std::endl;
             else if (sel == 1) std::cout << "[UI LOG] Create File" << std::endl;
        }
        else if (activeMenu == ActiveMenu::Insert) {
             int sel = dropdownAction.getSelectedIndex();
             std::string val = inputParam1.getText();
             if (sel == 0) std::cout << "[UI LOG] Insert Head | Val = " << val << std::endl;
             else if (sel == 1) std::cout << "[UI LOG] Insert Tail | Val = " << val << std::endl;
             else if (sel == 2) std::cout << "[UI LOG] Insert At | Val = " << val << " | Pos = " << inputParam2.getText() << std::endl;
        }
        else if (activeMenu == ActiveMenu::Remove) {
             int sel = dropdownAction.getSelectedIndex();
             if (sel == 0) std::cout << "[UI LOG] Delete Head" << std::endl;
             else if (sel == 1) std::cout << "[UI LOG] Delete Tail" << std::endl;
             else if (sel == 2) std::cout << "[UI LOG] Delete At | Pos = " << inputParam2.getText() << std::endl;
        }
        else if (activeMenu == ActiveMenu::Search) {
             std::cout << "[UI LOG] Search | Val = " << inputParam1.getText() << std::endl;
        }
        inputParam1.clear(); inputParam2.clear();
    }

    if (btnPrev.isClicked(event)) { }
    if (btnPlay.isClicked(event)) { }
    if (btnNext.isClicked(event)) { }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) ctx.nextState = ScreenState::MainMenu;
    }
}

void LinkedListScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    btnBack.update(mousePos);
    
    for (auto* btn : mainButtons) btn->update(mousePos);
    for (auto* input : activeInputs) input->update();
    for (auto* btn : activeSubButtons) btn->update(mousePos);
    
    if (activeMenu == ActiveMenu::Create || activeMenu == ActiveMenu::Insert || activeMenu == ActiveMenu::Remove) {
        dropdownAction.update(mousePos);
    }

    btnPrev.update(mousePos); 
    btnPlay.update(mousePos); 
    btnNext.update(mousePos);
}

void LinkedListScreen::draw() {
    ctx.window.draw(title);
    btnBack.draw();
    
    for (auto* btn : mainButtons) btn->draw();
    
    if (activeMenu != ActiveMenu::None && activeMenu != ActiveMenu::Clean) {
        ctx.window.draw(panelBg);
        
        for (auto* lbl : activeLabels) ctx.window.draw(*lbl);
        for (auto* input : activeInputs) input->draw();
        for (auto* btn : activeSubButtons) btn->draw();
        
        if (activeMenu == ActiveMenu::Create || activeMenu == ActiveMenu::Insert || activeMenu == ActiveMenu::Remove) {
             dropdownAction.draw();
        }
    }

    btnPrev.draw(); 
    btnPlay.draw(); 
    btnNext.draw();
}
