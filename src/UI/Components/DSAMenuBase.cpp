#include "UI/Components/DSAMenuBase.hpp"
#include <iostream>

namespace UI::Widgets {

DSAMenuBase::DSAMenuBase(AppContext& context, const std::string& titleText)
    : ctx(context),
      btnBack(context, " Back ", {20.f, 20.f}, {120.f, 50.f}),
      panelBg({300.f, 150.f}, Config::UI::Radius::Xl),
      btnPrev(context, "", {700.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
      btnPlay(context, "", {770.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
      btnNext(context, "", {840.f, context.window.getSize().y - 95.f}, {60.f, 40.f}),
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

    
    sf::Color panelColor(122, 160, 142);
    panelBg.setFillColor(panelColor);
    panelBg.setOutlineThickness(2.f);
    panelBg.setOutlineColor(sf::Color(200, 220, 200));

    speedSlider.setValue(50.f);

    sf::Vector2f prevPos = btnPrev.getPosition();
    iconPrev.setPosition({prevPos.x + 30.f, prevPos.y + 20.f});

    sf::Vector2f playPos = btnPlay.getPosition();
    iconPlay.setPosition({playPos.x + 30.f, playPos.y + 20.f});
    iconPause.setPosition({playPos.x + 30.f, playPos.y + 20.f});

    sf::Vector2f nextPos = btnNext.getPosition();
    iconNext.setPosition({nextPos.x + 30.f, nextPos.y + 20.f});
}

void DSAMenuBase::handleEvent(const sf::Event& event) {
    // btnBack handling is done via public API isBackClicked
    
    std::vector<std::string> labels = getMainButtonLabels();

    for (int i = 0; i < static_cast<int>(mainButtons.size()); ++i) {
        if (mainButtons[i].isClicked(event)) {
            saveCurrentInputsToCache();

            if (isInstantAction(i)) {
                activeMenuIndex = i;
                goClicked = true; 
            } else {
                activeMenuIndex = (activeMenuIndex == i) ? -1 : i;
                lastDropdownIndex = (activeMenuIndex == -1) ? -1 : 0;
            }

            updateLayout();
        }
    }

    if (dropdownAction && dropdownAction->isClicked(event)) {
        if (dropdownAction->getSelectedIndex() != lastDropdownIndex) {
            saveCurrentInputsToCache();
             dropdownAction->setLabel(dropdownAction->getSelectedText());
             lastDropdownIndex = dropdownAction->getSelectedIndex();
             updateLayout();
        }
    }

    if (!dropdownAction || !dropdownAction->getIsDropped()) {
        bool allInputsValid = true;
        bool submitted = false;

        for (auto& input : activeInputs) {
            input.handleEvent(event);

            if (!input.valid()) {
                allInputsValid = false;
            }

            if (input.isSubmitted(event)) {
                submitted = true;
            }
        }

        if (submitted && allInputsValid && !activeSubButtons.empty()) {
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
        }

        if (btnNext.isClicked(event)) {
            ctx.animManager.skipToEnd();
            ctx.animManager.setPaused(false);
        }

        if (btnPrev.isClicked(event)) {
            ctx.animManager.clearAll();
            ctx.animManager.setPaused(false);
            std::cout << "[INFO] Animation Cancelled.\n";

            cancelClicked = true;
        }
    } 
    else {
        if (ctx.animManager.isPaused()) {
            ctx.animManager.setPaused(false);
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
    
    if (activeMenuIndex != -1 && !isInstantAction(activeMenuIndex)) {
        window.draw(panelBg);
        for (auto& input : activeInputs) input.draw();
        for (auto& btn : activeSubButtons) btn.draw();
        if (dropdownAction) dropdownAction->draw();
    }

    
    if (!ctx.animManager.empty()) {
        btnPrev.draw();
        btnPlay.draw();
        btnNext.draw();

        if (btnPrev.isCurrentlyPressed()) iconPrev.move({0.f, 2.f});
        window.draw(iconPrev);
        if (btnPrev.isCurrentlyPressed()) iconPrev.move({0.f, -2.f});

        bool playPressed = btnPlay.isCurrentlyPressed();
        if (playPressed) {
            iconPlay.move({0.f, 2.f});
            iconPause.move({0.f, 2.f});
        }

        if (ctx.animManager.isPaused()) window.draw(iconPlay);
        else window.draw(iconPause);

        if (playPressed) {
            iconPlay.move({0.f, -2.f});
            iconPause.move({0.f, -2.f});
        }

        if (btnNext.isCurrentlyPressed()) iconNext.move({0.f, 2.f});
        window.draw(iconNext);
        if (btnNext.isCurrentlyPressed()) iconNext.move({0.f, -2.f});
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

    if (mainButtons.empty()) {
        for (const auto& label : labels) {
            mainButtons.emplace_back(ctx, label, sf::Vector2f{0.f, 0.f}, sf::Vector2f{buttonWidth, buttonHeight});
        }
    }

    for (int i = 0; i < static_cast<int>(mainButtons.size()); ++i) {
        auto& b = mainButtons[i];
        bool isActive = (activeMenuIndex == i);
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

    if (activeMenuIndex == -1 || isInstantAction(activeMenuIndex)) return;

    float boxX = 30.f;
    float boxY = mainY + buttonHeight + 15.f;
    if (activeMenuIndex >= 0 && activeMenuIndex < static_cast<int>(mainButtons.size())) {
        sf::Vector2f btnPos  = mainButtons[activeMenuIndex].getPosition();
        sf::Vector2f btnSize = mainButtons[activeMenuIndex].getSize();
        boxX = btnPos.x;
        boxY = btnPos.y + btnSize.y + 15.f;
    }

    renderSubMenu(boxX, boxY, activeMenuIndex);

    restoreInputsFromCache();
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
    activeMenuIndex = -1;
    lastDropdownIndex = -1;
    updateLayout();
}

void DSAMenuBase::clearInputs() {
    for (auto& input : activeInputs) input.clear();
}

std::string DSAMenuBase::makeInputCacheKey() const {
    return std::to_string(activeMenuIndex) + ":" + std::to_string(lastDropdownIndex);
}

void DSAMenuBase::saveCurrentInputsToCache() {
    if (activeMenuIndex == -1) return;

    std::vector<std::string> values;
    values.reserve(activeInputs.size());

    for (const auto& input : activeInputs) {
        values.push_back(input.getText());
    }

    inputTextCache[makeInputCacheKey()] = std::move(values);
}

void DSAMenuBase::restoreInputsFromCache() {
    auto it = inputTextCache.find(makeInputCacheKey());
    if (it == inputTextCache.end()) return;

    const auto& values = it->second;
    for (size_t i = 0; i < activeInputs.size() && i < values.size(); ++i) {
        activeInputs[i].setText(values[i]);
    }
}

}
