#include "UI/Components/DSAMenuBase.hpp"
#include <iostream>

namespace {

    enum class HeapStepType {
        InsertLastVisual,
        Compare,
        OverwriteValue,
        RemoveLastVisual,
        ApplyState
    };

    struct HeapStep {
        HeapStepType type;
        int i = -1;
        int j = -1;
        int value = 0;
        std::vector<int> state;
    };

    int findFirstIndex(const std::vector<int>& a, int val) {
        for (int i = 0; i < static_cast<int>(a.size()); ++i) {
            if (a[i] == val) return i;
        }
        return -1;
    }

    void traceHeapifyUp(std::vector<int>& a, int index, std::vector<HeapStep>& steps) {
        while (index > 0) {
            int p = (index - 1) / 2;

            steps.push_back({HeapStepType::Compare, p, index});

            if (a[p] < a[index]) {
                std::swap(a[p], a[index]);

                HeapStep s;
                s.type = HeapStepType::ApplyState;
                s.state = a;
                steps.push_back(std::move(s));

                index = p;
            } else {
                break;
            }
        }
    }

    void traceHeapifyDown(std::vector<int>& a, int index, std::vector<HeapStep>& steps) {
        int n = static_cast<int>(a.size());

        while (true) {
            int maxIndex = index;
            int left = 2 * index + 1;
            int right = 2 * index + 2;

            if (left < n) {
                steps.push_back({HeapStepType::Compare, index, left});
                if (a[left] > a[maxIndex]) maxIndex = left;
            }

            if (right < n) {
                steps.push_back({HeapStepType::Compare, maxIndex, right});
                if (a[right] > a[maxIndex]) maxIndex = right;
            }

            if (maxIndex != index) {
                std::swap(a[index], a[maxIndex]);

                HeapStep s;
                s.type = HeapStepType::ApplyState;
                s.state = a;
                steps.push_back(std::move(s));

                index = maxIndex;
            } else {
                break;
            }
        }
    }

    std::vector<HeapStep> buildInsertSteps(const std::vector<int>& initial, int val) {
        std::vector<int> a = initial;
        std::vector<HeapStep> steps;

        a.push_back(val);
        steps.push_back({HeapStepType::InsertLastVisual, -1, -1, val});

        traceHeapifyUp(a, static_cast<int>(a.size()) - 1, steps);
        return steps;
    }

    std::vector<HeapStep> buildDeleteSteps(const std::vector<int>& initial, int val) {
        std::vector<int> a = initial;
        std::vector<HeapStep> steps;

        int index = findFirstIndex(a, val);
        if (index == -1) return steps;

        int lastIndex = static_cast<int>(a.size()) - 1;

        if (index != lastIndex) {
            steps.push_back({HeapStepType::Compare, index, lastIndex});
            a[index] = a.back();
            steps.push_back({HeapStepType::OverwriteValue, index, -1, a[index]});
        }

        steps.push_back({HeapStepType::RemoveLastVisual});
        a.pop_back();

        if (index < static_cast<int>(a.size())) {
            int originalIndex = index;
            traceHeapifyUp(a, originalIndex, steps);
            traceHeapifyDown(a, originalIndex, steps);
        }

        return steps;
    }

    std::vector<HeapStep> buildUpdateSteps(const std::vector<int>& initial, int oldVal, int newVal) {
        std::vector<int> a = initial;
        std::vector<HeapStep> steps;

        int index = findFirstIndex(a, oldVal);
        if (index == -1) return steps;

        a[index] = newVal;
        steps.push_back({HeapStepType::OverwriteValue, index, -1, newVal});

        int originalIndex = index;
        traceHeapifyUp(a, originalIndex, steps);
        traceHeapifyDown(a, originalIndex, steps);

        return steps;
    }

} // namespace

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
    float buttonWidth = 200.f; 
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

int DSAMenuBase::consumeClickedSubButtonIndex() {
    int temp = clickedSubButtonIndex;
    clickedSubButtonIndex = -1;
    return temp;
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
