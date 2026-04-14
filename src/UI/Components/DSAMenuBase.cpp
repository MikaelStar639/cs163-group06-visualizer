#include "UI/Components/DSAMenuBase.hpp"
#include <iostream>

namespace UI::Widgets {

DSAMenuBase::DSAMenuBase(AppContext& context, const std::string& titleText)
    : ctx(context),
      btnBack(context, " Back ", {20.f, 20.f}, {120.f, 50.f}),
      panelBg({300.f, 150.f}, Config::UI::Radius::Xl),
      btnPrev(context, "|<", {700.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
      btnPlay(context, "||", {770.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
      btnNext(context, ">|", {840.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
      title(context.font, titleText, 24),
      speedSlider(context, 
                  sf::Vector2f{100.f, context.window.getSize().y - 80.f}, 
                  sf::Vector2f{300.f, 15.f})
{
    // Initialize Title
    title.setFillColor(Config::UI::Colors::ButtonHover);
    title.setStyle(sf::Text::Bold);
    title.setOrigin({0.f, 0.f});
    title.setPosition({160.f, 30.f}); 

    auto applyBtnColors = [](Button& b) {
        b.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, 
                    Config::UI::Colors::ButtonPressed, Config::UI::Colors::ButtonText);
    };

    applyBtnColors(btnBack); 
    applyBtnColors(btnPrev); applyBtnColors(btnPlay); applyBtnColors(btnNext);

    speedSlider.setValue(50.f);

    sf::Color panelColor(122, 160, 142);
    panelBg.setFillColor(panelColor);
    panelBg.setOutlineThickness(2.f);
    panelBg.setOutlineColor(sf::Color(200, 220, 200));
}

void DSAMenuBase::handleEvent(const sf::Event& event) {
    // btnBack handling is done via public API isBackClicked
    
    std::vector<std::string> labels = getMainButtonLabels();
    // Assuming labels correspond to ActiveMenu enums starting from Create
    std::vector<ActiveMenu> enums = {
        ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, 
        ActiveMenu::Search, ActiveMenu::Update, ActiveMenu::Clean
    };

    for (size_t i = 0; i < mainButtons.size(); ++i) {
        if (mainButtons[i].isClicked(event)) {
            ActiveMenu clickedMenu = enums[i];
            
            if (clickedMenu == ActiveMenu::Clean) {
                activeMenu = ActiveMenu::Clean;
                goClicked = true; 
            } else {
                activeMenu = (activeMenu == clickedMenu) ? ActiveMenu::None : clickedMenu;
                lastDropdownIndex = (activeMenu == ActiveMenu::None) ? -1 : 0;
            }
            
            updateLayout();
        }
    }

    if (dropdownAction && dropdownAction->isClicked(event)) {
        if (dropdownAction->getSelectedIndex() != lastDropdownIndex) {
             dropdownAction->setLabel(dropdownAction->getSelectedText());
             lastDropdownIndex = dropdownAction->getSelectedIndex();
             updateLayout();
        }
    }

    if (!dropdownAction || !dropdownAction->getIsDropped()) {
        bool enterPressed = false;
        if (const auto* keyEvent = event.getIf<sf::Event::KeyPressed>()) {
            if (keyEvent->code == sf::Keyboard::Key::Enter) {
                enterPressed = true;
            }
        }

        bool allInputsValid = true;
        for (auto& input : activeInputs) {
            input.handleEvent(event);
            if (!input.valid()) {
                allInputsValid = false;
            }
        }

        if (enterPressed && allInputsValid && !activeSubButtons.empty()) {
            goClicked = true;
            clickedSubButtonIndex = static_cast<int>(activeSubButtons.size() - 1);
            activeSubButtons[clickedSubButtonIndex].animateClick();
        }
    }

    if (!activeSubButtons.empty()) {
        for (size_t i = 0; i < activeSubButtons.size(); ++i) {
            if (activeSubButtons[i].isClicked(event)) {
                goClicked = true;
                clickedSubButtonIndex = static_cast<int>(i);
                break;
            }
        }
    }

    speedSlider.handleEvent(event);

    if (!ctx.animManager.empty()) {
        
        if (btnPlay.isClicked(event)) {
            ctx.animManager.togglePause();
            
            if (ctx.animManager.isPaused()) {
                btnPlay.setLabel(">");
            } else {
                btnPlay.setLabel("||");
            }
        }

        if (btnNext.isClicked(event)) {
            ctx.animManager.skipToEnd();
            ctx.animManager.setPaused(false);
            btnPlay.setLabel("||");
        }

        if (btnPrev.isClicked(event)) {
            ctx.animManager.clearAll();
            ctx.animManager.setPaused(false);
            btnPlay.setLabel("||");
            std::cout << "[INFO] Animation Cancelled.\n";

            cancelClicked = true;
        }
    } 
    else {
        if (ctx.animManager.isPaused()) {
            ctx.animManager.setPaused(false);
            btnPlay.setLabel("||");
        }
    }
}

void DSAMenuBase::update(sf::Vector2i mousePos) {
    btnBack.update(mousePos);
    for (auto& btn : mainButtons) btn.update(mousePos);
    for (auto& input : activeInputs) input.update();
    for (auto& btn : activeSubButtons) btn.update(mousePos);
    if (dropdownAction) dropdownAction->update(mousePos);
    
    if (!ctx.animManager.empty()) {
        btnPrev.update(mousePos);
        btnPlay.update(mousePos);
        btnNext.update(mousePos);
    }

    speedSlider.update(mousePos); 

    float sliderVal = speedSlider.getValue(); 
    
    float speed = 1.0f;
    if (sliderVal <= 50.f) {
        speed = 0.1f + (sliderVal / 50.f) * 0.9f;
    } else {
        speed = 1.0f + ((sliderVal - 50.f) / 50.f) * 2.0f;
    }
        
    ctx.animManager.setSpeedScale(speed);
}

void DSAMenuBase::draw(sf::RenderWindow& window) {
    window.draw(title);
    btnBack.draw();
    for (auto& btn : mainButtons) btn.draw();
    
    if (activeMenu != ActiveMenu::None && activeMenu != ActiveMenu::Clean) {
        window.draw(panelBg);
        for (auto& input : activeInputs) input.draw();
        for (auto& btn : activeSubButtons) btn.draw();
        if (dropdownAction) dropdownAction->draw();
    }

    
    if (!ctx.animManager.empty()) {
        btnPrev.draw();
        btnPlay.draw();
        btnNext.draw();
    }
    speedSlider.draw();
}

void DSAMenuBase::updateLayout() {
    float mainX = 30.f;      
    float mainY = 100.f;
    float gapMain = 5.f;    
    float buttonWidth = 170.f; 
    float buttonHeight = 60.f;

    std::vector<std::string> labels = getMainButtonLabels();
    std::vector<ActiveMenu> enums = {
        ActiveMenu::Create, ActiveMenu::Insert, ActiveMenu::Remove, 
        ActiveMenu::Search, ActiveMenu::Update, ActiveMenu::Clean
    };

    if (mainButtons.empty()) {
        for (const auto& label : labels) {
            mainButtons.emplace_back(ctx, label, sf::Vector2f{0.f, 0.f}, sf::Vector2f{buttonWidth, buttonHeight});
        }
    }

    for (size_t i = 0; i < mainButtons.size(); ++i) {
        auto& b = mainButtons[i];
        bool isActive = (activeMenu == enums[i]);
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

    float boxX = 30.f;
    float boxY = mainY + buttonHeight + 15.f;
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

bool DSAMenuBase::consumeGoClicked() {
    if (goClicked) {
        goClicked = false;
        return true;
    }
    return false;
}

bool DSAMenuBase::consumeCancelClicked() { 
    if (cancelClicked) { 
        cancelClicked = false; 
        return true; 
    } 
    return false; 
}

void DSAMenuBase::resetMenu() {
    activeMenu = ActiveMenu::None;
    lastDropdownIndex = -1;
    updateLayout();
}

void DSAMenuBase::clearInputs() {
    for (auto& input : activeInputs) input.clear();
}

}
