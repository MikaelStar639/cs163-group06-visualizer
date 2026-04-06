#include "States/LinkedListScreen.hpp"
#include "Core/Constants.hpp"
#include <iostream>

LinkedListScreen::LinkedListScreen(AppContext& context)
    : ctx(context),
      btnBack(context, " Back ", {20.f, 20.f}, {120.f, 50.f}),
      panelBg({300.f, 150.f}, Config::UI::BUTTON_CORNER_RADIUS),
      btnPrev(context, "|<", {700.f, 840.f}, {60.f, 40.f}),
      btnPlay(context, "||", {770.f, 840.f}, {60.f, 40.f}),
      btnNext(context, ">|", {840.f, 840.f}, {60.f, 40.f}),
      title(context.font, "Linked List", 24)
{
    mainButtons.emplace_back(context, "Create", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Insert", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Delete", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Search", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Update", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Set Radius", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});
    mainButtons.emplace_back(context, "Clear All", sf::Vector2f{0.f, 0.f}, sf::Vector2f{180.f, 60.f});

    initUI();
    updateLayout();
}

void LinkedListScreen::initUI() {
    title.setFillColor(Config::UI::Colors::ButtonHover);
    title.setStyle(sf::Text::Bold);
    title.setOrigin({0.f, 0.f});
    title.setPosition({160.f, 30.f}); 

    auto applyBtnColors = [](UI::Widgets::Button& b) {
        b.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, 
                    Config::UI::Colors::ButtonPressed, Config::UI::Colors::ButtonText);
    };

    applyBtnColors(btnBack); 
    btnBack.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, Config::UI::Colors::ButtonPressed, sf::Color(255, 50, 50));
    applyBtnColors(btnPrev); applyBtnColors(btnPlay); applyBtnColors(btnNext);

    sf::Color panelColor(122, 160, 142);
    panelBg.setFillColor(panelColor);
    panelBg.setOutlineThickness(2.f);
    panelBg.setOutlineColor(sf::Color(200, 220, 200));
}

void LinkedListScreen::renderSubMenu(float boxX, float boxY, ActiveMenu type) {
    
    float innerX = boxX + 15.f;
    float innerY = boxY + 15.f;
    float boxHeight = 80.f;
    float boxWidth = 0.f;

    activeSubButtons.clear();
    activeInputs.clear();
    dropdownAction.reset();

    sf::Color innerBtnIdle = Config::UI::Colors::ButtonIdle;
    sf::Color innerBtnHover = Config::UI::Colors::ButtonHover;
    sf::Color innerBtnPress = Config::UI::Colors::ButtonPressed;
    sf::Color dropdownHover = Config::UI::Colors::ButtonHover;
    sf::Color dropdownPress = Config::UI::Colors::ButtonPressed;

    auto createDropdown = [&](const std::vector<std::string>& options, float x, float w) {
        dropdownAction.emplace(ctx, "Select...", sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f});
        dropdownAction->setColors(innerBtnIdle, dropdownHover, dropdownPress, sf::Color::White);
        dropdownAction->setOptions(options);
        
        if (lastDropdownIndex >= 0 && lastDropdownIndex < static_cast<int>(options.size())) {
            dropdownAction->setSelectedIndex(lastDropdownIndex);
        } else {
            dropdownAction->setSelectedIndex(0);
            lastDropdownIndex = 0;
        }

        dropdownAction->setLabel(dropdownAction->getSelectedText());
        return dropdownAction->getSelectedIndex();
    };

    auto createInput = [&](const std::string& placeholder, float x, float w,  UI::Widgets::InputType inputType = UI::Widgets::InputType::Integer) {
        activeInputs.emplace_back(ctx, sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f}, "", inputType);
        activeInputs.back().setPlaceholder(placeholder);
    };

    auto createExecuteBtn = [&](float x) {
        activeSubButtons.emplace_back(ctx, "Go", sf::Vector2f{x, innerY}, sf::Vector2f{90.f, 45.f});
        activeSubButtons.back().setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
    };

    float currentX = innerX;
    float gap = 15.f; // Increased gap for better spacing

    if (type == ActiveMenu::Create) {
        int sel = createDropdown({"Random", "File"}, currentX, 160.f);
        currentX += 160.f + gap;

        if (sel == 0) { // Random
            createInput("Size", currentX, 120.f);
            currentX += 120.f + gap;
        }else if (sel == 1) { // File
            createInput("File path", currentX, 500.f, UI::Widgets::InputType::AnyText);
            currentX += 500.f + gap;
        }
        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == ActiveMenu::Insert) {
        int sel = createDropdown({"Head", "Tail", "At"}, currentX, 160.f);
        currentX += 160.f + gap;

        createInput("Value", currentX, 120.f);
        currentX += 120.f + gap;

        if (sel == 2) { // At
            createInput("Pos", currentX, 100.f);
            currentX += 100.f + gap;
        }

        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == ActiveMenu::Remove) {
        int sel = createDropdown({"Head", "Tail", "At"}, currentX, 160.f);
        currentX += 160.f + gap;

        if (sel == 2) { // At
            createInput("Pos", currentX, 100.f);
            currentX += 100.f + gap;
        }

        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == ActiveMenu::Search) {
        createInput("Value", currentX, 160.f);
        currentX += 160.f + gap;
        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == ActiveMenu::Update) {
        int sel = createDropdown({"At", "By Value"}, currentX, 190.f);
        currentX += 190.f + gap;
        
        if (sel == 0) { // At
            createInput("Pos", currentX, 120.f);
            currentX += 120.f + gap;
            createInput("New Val", currentX, 150.f);
            currentX += 150.f + gap;
        } else if (sel == 1) { // By Value
            createInput("Old Val", currentX, 150.f);
            currentX += 150.f + gap;
            createInput("New Val", currentX, 150.f);
            currentX += 150.f + gap;
        }

        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == ActiveMenu::SetRadius) {
        createInput("Radius", currentX, 150.f);
        currentX += 150.f + gap;
        createExecuteBtn(currentX);
        currentX += 90.f;
    }

    boxWidth = (currentX - boxX) + 20.f; // Slightly more right-padding 

    panelBg.setPosition({boxX, boxY});
    panelBg.setSize({boxWidth, boxHeight});

    
    float windowWidth = static_cast<float>(ctx.window.getSize().x);

    if (boxX + boxWidth > windowWidth - 20.f) {
        boxX = windowWidth - boxWidth - 20.f;
        innerX = boxX + 15.f;
    }
}

void LinkedListScreen::updateLayout() {
    float mainX = 30.f;      
    float mainY = 100.f;
    float gapMain = 5.f;    
    float buttonWidth = 170.f; // Nới rộng các nút để text không bị chèn lấp
    float buttonHeight = 60.f;

    // --- Place Main Buttons Horizontally  ---
    ActiveMenu enums[] = {ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, ActiveMenu::Search, ActiveMenu::Update, ActiveMenu::SetRadius, ActiveMenu::Clean};
    
    for (size_t i = 0; i < mainButtons.size(); ++i) {
        auto& b = mainButtons[i];
        bool isActive = (activeMenu == enums[i]);
        b.setSize({buttonWidth, buttonHeight});
        b.setPosition({mainX + (buttonWidth + gapMain) * static_cast<float>(i), mainY});
        
        if (isActive) {
            b.setColors(sf::Color(122, 160, 142), sf::Color(122, 160, 142), sf::Color(122, 160, 142), sf::Color::White);
        } else {
            b.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, Config::UI::Colors::ButtonPressed, Config::UI::Colors::ButtonText);
        }
    }

    activeSubButtons.clear();
    activeInputs.clear();
    dropdownAction.reset();

    if (activeMenu == ActiveMenu::None || activeMenu == ActiveMenu::Clean) return;

    // --- Panel Settings ---
    float boxX = 30.f;
    float boxY = mainY + 55.f + 15.f; // Panels stay under buttons
    for (size_t i = 0; i < mainButtons.size(); ++i) {
        if (activeMenu == enums[i]) {
            sf::Vector2f btnPos  = mainButtons[i].getPosition();
            sf::Vector2f btnSize = mainButtons[i].getSize();

            boxX = btnPos.x;
            boxY = btnPos.y + btnSize.y + 15.f;
            break;
        }
    }

    renderSubMenu(boxX, boxY, activeMenu);
}

void LinkedListScreen::handleEvent(const sf::Event& event) {
    if (btnBack.isClicked(event)) ctx.nextState = ScreenState::MainMenu;

    ActiveMenu enums[] = {ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, ActiveMenu::Search, ActiveMenu::Update, ActiveMenu::SetRadius, ActiveMenu::Clean};
    for (size_t i = 0; i < mainButtons.size(); ++i) {
        if (mainButtons[i].isClicked(event)) {
            activeMenu = (activeMenu == enums[i]) ? ActiveMenu::None : enums[i];
            
            lastDropdownIndex = 0;
            
            if (activeMenu == ActiveMenu::Clean) {
                std::cout << "[UI LOG] Action executed: Clear All" << std::endl;
                // -------------------------------------------------------------
                // [TODO] CODE LOGIC Ở ĐÂY: XOÁ TOÀN BỘ LINKED LIST (CLEAR)
                // -------------------------------------------------------------
                activeMenu = ActiveMenu::None; 
            }
            updateLayout();
        }
    }

    if (activeMenu == ActiveMenu::Create || activeMenu == ActiveMenu::Insert || activeMenu == ActiveMenu::Remove || activeMenu == ActiveMenu::Update) {
        if (dropdownAction && dropdownAction->isClicked(event)) {
            if (dropdownAction->getSelectedIndex() != lastDropdownIndex) {
                 dropdownAction->setLabel(dropdownAction->getSelectedText());
                 lastDropdownIndex = dropdownAction->getSelectedIndex();
                 updateLayout();
            }
        }
    }

    if (!dropdownAction || !dropdownAction->getIsDropped()) {
        for (auto& input : activeInputs) {
            input.handleEvent(event);
        }
    }

    if (!activeSubButtons.empty() && activeSubButtons[0].isClicked(event)) {
        if (activeMenu == ActiveMenu::Create) {
             int sel = dropdownAction ? dropdownAction->getSelectedIndex() : -1;
             if (sel == 0) {
                 std::cout << "[UI LOG] Create Random | Size = " << (!activeInputs.empty() ? activeInputs[0].getText() : "") << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: KHỞI TẠO LINKED LIST NGẪU NHIÊN 
                 // (Dùng biến 'size' lấy từ activeInputs[0].getText())
                 // -------------------------------------------------------------
             }
             else if (sel == 1) {
                 std::cout << "[UI LOG] Create File | Path = " << (!activeInputs.empty() ? activeInputs[0].getText() : "") << std::endl;
                 std::string path = !activeInputs.empty() ? activeInputs[0].getText() : "";
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: ĐỌC TỪ FILE VÀ KHỞI TẠO LINKED LIST
                 // (File path đang ở biến 'path')
                 // -------------------------------------------------------------
             }
        }
        else if (activeMenu == ActiveMenu::Insert) {
             int sel = dropdownAction ? dropdownAction->getSelectedIndex() : -1;
             std::string val = !activeInputs.empty() ? activeInputs[0].getText() : "";
             
             if (sel == 0) {
                 std::cout << "[UI LOG] Insert Head | Val = " << val << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: CHÈN PHẦN TỬ VÀO ĐẦU (INSERT HEAD)
                 // -------------------------------------------------------------
             }
             else if (sel == 1) {
                 std::cout << "[UI LOG] Insert Tail | Val = " << val << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: CHÈN PHẦN TỬ VÀO CUỐI (INSERT TAIL)
                 // -------------------------------------------------------------
             }
             else if (sel == 2) {
                 std::string pos = activeInputs.size() > 1 ? activeInputs[1].getText() : "";
                 std::cout << "[UI LOG] Insert At | Val = " << val << " | Pos = " << pos << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: CHÈN PHẦN TỬ VÀO VỊ TRÍ 'POS' (INSERT AT)
                 // -------------------------------------------------------------
             }
        }
        else if (activeMenu == ActiveMenu::Remove) {
             int sel = dropdownAction ? dropdownAction->getSelectedIndex() : -1;
             
             if (sel == 0) {
                 std::cout << "[UI LOG] Delete Head" << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: XOÁ PHẦN TỬ Ở ĐẦU (DELETE HEAD)
                 // -------------------------------------------------------------
             }
             else if (sel == 1) {
                 std::cout << "[UI LOG] Delete Tail" << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: XOÁ PHẦN TỬ Ở CUỐI (DELETE TAIL)
                 // -------------------------------------------------------------
             }
             else if (sel == 2) {
                 std::string pos = !activeInputs.empty() ? activeInputs[0].getText() : "";
                 std::cout << "[UI LOG] Delete At | Pos = " << pos << std::endl;
                 // -------------------------------------------------------------
                 // [TODO] CODE LOGIC Ở ĐÂY: XOÁ PHẦN TỬ Ở VỊ TRÍ 'POS' (DELETE AT)
                 // -------------------------------------------------------------
             }
        }
        else if (activeMenu == ActiveMenu::Search) {
             std::string val = !activeInputs.empty() ? activeInputs[0].getText() : "";
             std::cout << "[UI LOG] Search | Val = " << val << std::endl;
             // -------------------------------------------------------------
             // [TODO] CODE LOGIC Ở ĐÂY: TÌM KIẾM PHẦN TỬ THEO 'VAL' (SEARCH)
             // -------------------------------------------------------------
        }
        else if (activeMenu == ActiveMenu::Update) {
             int sel = dropdownAction ? dropdownAction->getSelectedIndex() : -1;
             if (sel == 0) {
                 std::string pos = activeInputs.size() > 0 ? activeInputs[0].getText() : "";
                 std::string newVal = activeInputs.size() > 1 ? activeInputs[1].getText() : "";
                 std::cout << "[UI LOG] Update At | Pos = " << pos << " | New Val = " << newVal << std::endl;
             }
             else if (sel == 1) {
                 std::string oldVal = activeInputs.size() > 0 ? activeInputs[0].getText() : "";
                 std::string newVal = activeInputs.size() > 1 ? activeInputs[1].getText() : "";
                 std::cout << "[UI LOG] Update By Value | Old Val = " << oldVal << " | New Val = " << newVal << std::endl;
             }
        }
        else if (activeMenu == ActiveMenu::SetRadius) {
             std::string radStr = !activeInputs.empty() ? activeInputs[0].getText() : "";
             std::cout << "[UI LOG] Set Radius to: " << radStr << std::endl;
             // [TODO]: Apply radius setting
        }
        
        for (auto& input : activeInputs) input.clear();
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
    
    for (auto& btn : mainButtons) btn.update(mousePos);
    for (auto& input : activeInputs) input.update();
    for (auto& btn : activeSubButtons) btn.update(mousePos);
    
    if (dropdownAction) {
        dropdownAction->update(mousePos);
    }

    btnPrev.update(mousePos); 
    btnPlay.update(mousePos); 
    btnNext.update(mousePos);
}

void LinkedListScreen::draw() {
    ctx.window.draw(title);
    btnBack.draw();
    
    for (auto& btn : mainButtons) btn.draw();
    
    if (activeMenu != ActiveMenu::None && activeMenu != ActiveMenu::Clean) {
        ctx.window.draw(panelBg);
        
        for (auto& input : activeInputs) input.draw();
        for (auto& btn : activeSubButtons) btn.draw();
        
        if (dropdownAction) {
             dropdownAction->draw();
        }
    }

    btnPrev.draw(); 
    btnPlay.draw(); 
    btnNext.draw();
}
