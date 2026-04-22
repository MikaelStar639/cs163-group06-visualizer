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
      btnToggleStepMode(context, "Step: OFF", {480.f, context.window.getSize().y - 95.f}, {160.f, 40.f}),
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
    applyBtnColors(btnToggleStepMode);

    
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
    // 1. Initialize enabled state vector if not already done
    if (mainButtonsEnabled.size() != mainButtons.size()) {
        mainButtonsEnabled.assign(mainButtons.size(), true);
    }

    // 2. Main Button Handling with Locking Guard
    for (int i = 0; i < static_cast<int>(mainButtons.size()); ++i) {
        if (mainButtons[i].isClicked(event)) {
            // THE LOCK: If disabled, ignore the click entirely
            if (!mainButtonsEnabled[i]) {
                continue; 
            }

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

    // 3. Dropdown Handling
    if (dropdownAction && dropdownAction->isClicked(event)) {
        if (dropdownAction->getSelectedIndex() != lastDropdownIndex) {
            saveCurrentInputsToCache();
             dropdownAction->setLabel(dropdownAction->getSelectedText());
             lastDropdownIndex = dropdownAction->getSelectedIndex();
             updateLayout();
        }
    }

    // 4. Input and Enter Key Handling
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

    // 5. Sub-button (Execute/Go) Handling
    if (!activeSubButtons.empty()) {
        for (size_t i = 0; i < activeSubButtons.size(); ++i) {
            if (activeSubButtons[i].isClicked(event)) {
                goClicked = true;
                clickedSubButtonIndex = static_cast<int>(i);
                break;
            }
        }
    }

    // 6. Timeline and Speed Controls
    speedSlider.handleEvent(event);

    // Allow controls if we are animating OR if we have steps to play/back
    bool isAtEnd = !ctx.stepNavigator.hasNext() && ctx.animManager.empty();
    bool shouldShowControls = !isAtEnd && ctx.stepNavigator.getTotalSteps() > 0;
    
    // In Step Mode, allow seeing the bar to step back, but hide if we've reached the very end
    if (ctx.isStepByStep && ctx.stepNavigator.getTotalSteps() > 0) {
        if (isAtEnd && ctx.stepNavigator.getCurrentIndex() >= ctx.stepNavigator.getTotalSteps() - 1) {
            shouldShowControls = false;
        } else {
            shouldShowControls = true;
        }
    }

    if (shouldShowControls) {
        if (btnPlay.isClicked(event)) {
            ctx.animManager.togglePause();   
        }

        if (btnNext.isClicked(event)) {
            if (ctx.stepNavigator.hasNext()) {
                ctx.stepNavigator.playNext();
            } else if (!ctx.isStepByStep) {
                // Only skip to end if we are in continuous mode
                ctx.animManager.skipToEnd();
                ctx.animManager.setPaused(false);
            }
        }

        if (btnPrev.isClicked(event)) {
            if (ctx.isStepByStep) {
                if (ctx.stepNavigator.getCurrentIndex() >= 0) {
                    // Nhấn lần đầu: Về Step -1
                    ctx.stepNavigator.stepBack();
                } else {
                    // Đã ở Step -1, nhấn lần nữa sẽ Cancel (đây chính là Step -2)
                    ctx.animManager.clearAll();
                    ctx.stepNavigator.clear();
                    ctx.animManager.setPaused(false);
                    std::cout << "[INFO] Animation Cancelled.\n";
                    cancelClicked = true;
                }
            } else {
                // Chế độ Step OFF: Cancel luôn
                ctx.animManager.clearAll();
                ctx.stepNavigator.clear();
                ctx.animManager.setPaused(false);
                std::cout << "[INFO] Animation Cancelled.\n";
                cancelClicked = true;
            }
        }
    } 
    else {
        // Reset pause state when done, but ONLY if not in Step Mode
        if (ctx.animManager.isPaused() && !ctx.isStepByStep) {
            ctx.animManager.setPaused(false);
        }
    }

    if (btnBack.isClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    if (btnToggleStepMode.isClicked(event)) {
        ctx.isStepByStep = !ctx.isStepByStep;
        btnToggleStepMode.setLabel(ctx.isStepByStep ? "Step: ON" : "Step: OFF");
        ctx.stepNavigator.setStepMode(ctx.isStepByStep);

        if (ctx.isStepByStep) {
            ctx.animManager.setPaused(true);
        } else {
            ctx.animManager.setPaused(false);
        }
    }
}

void DSAMenuBase::update(sf::Vector2i mousePos) {
    ctx.stepNavigator.update(0.f); // Autonomous advancement
    btnBack.update(mousePos);
    for (auto& btn : mainButtons) btn.update(mousePos);
    for (auto& input : activeInputs) input.update();
    for (auto& btn : activeSubButtons) btn.update(mousePos);
    if (dropdownAction) dropdownAction->update(mousePos);
    
    bool isAtEnd = !ctx.stepNavigator.hasNext() && ctx.animManager.empty();
    bool shouldShowControls = !isAtEnd && ctx.stepNavigator.getTotalSteps() > 0;
    if (ctx.isStepByStep && ctx.stepNavigator.getTotalSteps() > 0) {
        if (isAtEnd && ctx.stepNavigator.getCurrentIndex() >= ctx.stepNavigator.getTotalSteps() - 1) {
            shouldShowControls = false;
        } else {
            shouldShowControls = true;
        }
    }

    if (shouldShowControls) {
        btnPrev.update(mousePos);
        btnPlay.update(mousePos);
        btnNext.update(mousePos);
    }

    btnToggleStepMode.update(mousePos);
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

    
    bool isAtEnd = !ctx.stepNavigator.hasNext() && ctx.animManager.empty();
    bool shouldShowControls = !isAtEnd && ctx.stepNavigator.getTotalSteps() > 0;
    if (ctx.isStepByStep && ctx.stepNavigator.getTotalSteps() > 0) {
        if (isAtEnd && ctx.stepNavigator.getCurrentIndex() >= ctx.stepNavigator.getTotalSteps() - 1) {
            shouldShowControls = false;
        } else {
            shouldShowControls = true;
        }
    }

    if (shouldShowControls) {
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

    btnToggleStepMode.draw();
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

bool DSAMenuBase::consumeSkipClicked() { 
    if (skipClicked) { 
        skipClicked = false; 
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


void DSAMenuBase::setMainButtonEnabled(int index, bool enabled) {
    // 1. Ensure the state vector is synchronized with the buttons
    if (mainButtonsEnabled.size() != mainButtons.size()) {
        mainButtonsEnabled.assign(mainButtons.size(), true);
    }

    if (index < 0 || index >= static_cast<int>(mainButtons.size())) return;

    mainButtonsEnabled[index] = enabled;

    // 2. Apply visual feedback immediately
    if (!enabled) {
        sf::Color grey(70, 70, 70);
        mainButtons[index].setColors(grey, grey, grey, Config::UI::Colors::ButtonText);
    } else {
        mainButtons[index].setColors(
            Config::UI::Colors::ButtonIdle, 
            Config::UI::Colors::ButtonHover, 
            Config::UI::Colors::ButtonPressed, 
            Config::UI::Colors::ButtonText
        );
    }
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

void DSAMenuBase::setCachedInputsForState(int menuIndex, int dropdownIndex, const std::vector<std::string>& values) {
    std::string key = std::to_string(menuIndex) + ":" + std::to_string(dropdownIndex);
    inputTextCache[key] = values;

    // nếu đang đứng đúng submenu đó thì update luôn input đang hiển thị
    if (activeMenuIndex == menuIndex && lastDropdownIndex == dropdownIndex) {
        for (size_t i = 0; i < activeInputs.size() && i < values.size(); ++i) {
            activeInputs[i].setText(values[i]);
        }
    }
}

}
