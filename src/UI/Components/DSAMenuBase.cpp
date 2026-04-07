#include "UI/Components/DSAMenuBase.hpp"
#include <iostream>

namespace UI::Widgets {

DSAMenuBase::DSAMenuBase(AppContext& context, const std::string& titleText)
    : ctx(context),
      btnBack(context, " Back ", {20.f, 20.f}, {120.f, 50.f}),
      panelBg({300.f, 150.f}, Config::UI::Radius::Xl),
      btnPrev(context, "|<", {700.f, 840.f}, {60.f, 40.f}),
      btnPlay(context, "||", {770.f, 840.f}, {60.f, 40.f}),
      btnNext(context, ">|", {840.f, 840.f}, {60.f, 40.f}),
      title(context.font, titleText, 24)
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
    btnBack.setColors(Config::UI::Colors::ButtonIdle, Config::UI::Colors::ButtonHover, Config::UI::Colors::ButtonPressed, sf::Color(255, 50, 50));
    applyBtnColors(btnPrev); applyBtnColors(btnPlay); applyBtnColors(btnNext);

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
            activeMenu = (activeMenu == enums[i]) ? ActiveMenu::None : enums[i];
            lastDropdownIndex = (activeMenu == ActiveMenu::None) ? -1 : 0;
            
            if (activeMenu == ActiveMenu::Clean) {
                goClicked = true; // Signal Clean action
                activeMenu = ActiveMenu::None;
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
        for (auto& input : activeInputs) {
            input.handleEvent(event);
        }
    }

    if (!activeSubButtons.empty() && activeSubButtons[0].isClicked(event)) {
        goClicked = true;
        // logic for clearing inputs is moved to consumeGoClicked or clearInputs
    }
}

void DSAMenuBase::update(sf::Vector2i mousePos) {
    btnBack.update(mousePos);
    for (auto& btn : mainButtons) btn.update(mousePos);
    for (auto& input : activeInputs) input.update();
    for (auto& btn : activeSubButtons) btn.update(mousePos);
    if (dropdownAction) dropdownAction->update(mousePos);
    
    btnPrev.update(mousePos); 
    btnPlay.update(mousePos); 
    btnNext.update(mousePos);
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

    btnPrev.draw(); 
    btnPlay.draw(); 
    btnNext.draw();
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

void DSAMenuBase::clearInputs() {
    for (auto& input : activeInputs) input.clear();
}

}
