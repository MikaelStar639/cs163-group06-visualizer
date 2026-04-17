#include "UI/Components/LinkedListMenu.hpp"

namespace UI::Widgets {

LinkedListMenu::LinkedListMenu(AppContext& context)
    : DSAMenuBase(context, "Linked List")
{
    updateLayout();
}

std::vector<std::string> LinkedListMenu::getMainButtonLabels() const {
    return {"Create", "Insert", "Delete", "Search", "Update", "Clear All"};
}

bool LinkedListMenu::isInstantAction(int index) const {
    return index == static_cast<int>(Action::Clean);
}

void LinkedListMenu::renderSubMenu(float boxX, float boxY, int menuIndex) {
    float innerX = boxX + 15.f;
    float innerY = boxY + 15.f;
    float boxHeight = 80.f;
    float boxWidth = 0.f;

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

    auto createInput = [&](const std::string& placeholder, float x, float w,  InputType inputType = InputType::AnyText) {
        activeInputs.emplace_back(ctx, sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f}, "", inputType);
        activeInputs.back().setPlaceholder(placeholder);
    };

    auto createExecuteBtn = [&](float x) {
        activeSubButtons.emplace_back(ctx, "Go", sf::Vector2f{x, innerY}, sf::Vector2f{90.f, 45.f});
        activeSubButtons.back().setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
    };

    float currentX = innerX;
    float gap = 15.f; 
    Action type = static_cast<Action>(menuIndex);

    if (type == Action::Create) {
        int sel = createDropdown({"Random", "File"}, currentX, 160.f);
        currentX += 160.f + gap;

        if (sel == 0) { // Random
            createInput("Size", currentX, 120.f, InputType::Integer);
            currentX += 120.f + gap;
            createExecuteBtn(currentX);
            currentX += 90.f;
        } else if (sel == 1) { // File
            // Create "Edit" button
            activeSubButtons.emplace_back(ctx, "Edit", sf::Vector2f{currentX, innerY}, sf::Vector2f{90.f, 45.f});
            activeSubButtons.back().setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
            currentX += 90.f + gap;
            
            // Create "Go" button
            activeSubButtons.emplace_back(ctx, "Go", sf::Vector2f{currentX, innerY}, sf::Vector2f{90.f, 45.f});
            activeSubButtons.back().setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
            currentX += 90.f;
        }
    }
    else if (type == Action::Insert) {
        int sel = createDropdown({"Head", "Tail", "At"}, currentX, 160.f);
        currentX += 160.f + gap;

        if (sel == 2) { // At
            createInput("Pos", currentX, 100.f, InputType::Integer);
            currentX += 100.f + gap;
        }

        createInput("Value", currentX, 120.f, InputType::Integer);
        currentX += 120.f + gap;

        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == Action::Delete) {
        int sel = createDropdown({"Head", "Tail", "At"}, currentX, 160.f);
        currentX += 160.f + gap;

        if (sel == 2) { // At
            createInput("Pos", currentX, 100.f, InputType::Integer);
            currentX += 100.f + gap;
        }

        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == Action::Search) {
        createInput("Value", currentX, 160.f, InputType::Integer);
        currentX += 160.f + gap;
        createExecuteBtn(currentX);
        currentX += 90.f;
    }
    else if (type == Action::Update) {
        int sel = createDropdown({"At", "By Value"}, currentX, 190.f);
        currentX += 190.f + gap;
        
        if (sel == 0) { // At
            createInput("Pos", currentX, 120.f, InputType::Integer);
            currentX += 120.f + gap;
            createInput("New Val", currentX, 150.f, InputType::Integer);
            currentX += 150.f + gap;
        } else if (sel == 1) { // By Value
            createInput("Old Val", currentX, 150.f, InputType::Integer);
            currentX += 150.f + gap;
            createInput("New Val", currentX, 150.f, InputType::Integer);
            currentX += 150.f + gap;
        }

        createExecuteBtn(currentX);
        currentX += 90.f;
    }

    boxWidth = (currentX - boxX) + 20.f; 

    panelBg.setPosition({boxX, boxY});
    panelBg.setSize({boxWidth, boxHeight});

    float windowWidth = static_cast<float>(ctx.window.getSize().x);
    if (boxX + boxWidth > windowWidth - 20.f) {
        boxX = windowWidth - boxWidth - 20.f;
        panelBg.setPosition({boxX, boxY});
    }
}

}
