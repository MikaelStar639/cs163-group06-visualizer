#include "UI/Components/HeapMenu.hpp"

namespace UI::Widgets {

HeapMenu::HeapMenu(AppContext& context)
    : DSAMenuBase(context, "Heap")
{
    updateLayout();
}

std::vector<std::string> HeapMenu::getMainButtonLabels() const {
    // Indices: 0: Create, 1: Insert, 2: Delete Root, 3: Find Maximum, 4: Clear All
    return {"Create", "Insert", "Delete Root", "Find Maximum", "Clear All"};
}

void HeapMenu::setMainButtonEnabled(int index, bool enabled) {
    if (index < 0 || index >= static_cast<int>(mainButtons.size())) return;

    if (!enabled) {
        // Grey out: All states (idle, hover, pressed) become dark grey
        sf::Color grey(70, 70, 70);
        mainButtons[index].setColors(grey, grey, grey, Config::UI::Colors::ButtonText);
    } else {
        // Restore: Reset to original theme colors
        mainButtons[index].setColors(
            Config::UI::Colors::ButtonIdle,
            Config::UI::Colors::ButtonHover,
            Config::UI::Colors::ButtonPressed,
            Config::UI::Colors::ButtonText
        );
    }
}

void HeapMenu::renderSubMenu(float boxX, float boxY, ActiveMenu type) {
    float innerX = boxX + 15.f;
    float innerY = boxY + 15.f;
    float boxHeight = 80.f;
    float boxWidth = 0.f;

    sf::Color idle = Config::UI::Colors::ButtonIdle;
    sf::Color hover = Config::UI::Colors::ButtonHover;
    sf::Color press = Config::UI::Colors::ButtonPressed;

    auto createInput = [&](const std::string& placeholder, float x, float w) {
        activeInputs.emplace_back(ctx, sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f}, "", InputType::Integer);
        activeInputs.back().setPlaceholder(placeholder);
    };

    auto createCustomBtn = [&](const std::string& label, float x, float w = 100.f) { // Increased default width to 100.f
        activeSubButtons.emplace_back(ctx, label, sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f});
        activeSubButtons.back().setColors(idle, hover, press, sf::Color::White);
    };

    float currentX = innerX;
    float gap = 50.f; // Increased gap for better breathing room

    if (type == ActiveMenu::Create) {
        int sel = lastDropdownIndex; 
        
        dropdownAction.emplace(ctx, "Select...", sf::Vector2f{currentX, innerY}, sf::Vector2f{160.f, 45.f});
        dropdownAction->setColors(idle, hover, press, sf::Color::White);
        dropdownAction->setOptions({"Random", "File"});
        dropdownAction->setSelectedIndex(sel >= 0 ? sel : 0);
        dropdownAction->setLabel(dropdownAction->getSelectedText());
        
        currentX += 160.f + gap;

        if (sel == 0) { // Random
            createInput("Size", currentX, 100.f);
            currentX += 100.f + gap;
        } 
        else if (sel == 1) { // File
            createCustomBtn("Edit", currentX, 100.f);
            currentX += 100.f + gap;
        }

        createCustomBtn("Create", currentX, 110.f);
        currentX += 110.f + gap;
        createCustomBtn("Heapify", currentX, 110.f);
        currentX += 110.f;
    }
    else if (type == ActiveMenu::Insert) {
        createInput("Value", currentX, 140.f);
        currentX += 140.f + gap;
        createCustomBtn("Insert", currentX, 110.f);
        currentX += 110.f;
    }
    else if (type == ActiveMenu::Remove) {
        createCustomBtn("Go", currentX, 100.f);
        currentX += 100.f;
    }
    else if (type == ActiveMenu::Search) {
        createCustomBtn("Go", currentX, 100.f);
        currentX += 100.f;
    }
    // --- Mapped to the 5th button (Clear All) as requested ---
    else if (type == ActiveMenu::Update) {
        createCustomBtn("Go", currentX, 100.f);
        currentX += 100.f;
    }

    boxWidth = (currentX - boxX) + 25.f; // Added extra padding to prevent edge clipping
    panelBg.setPosition({boxX, boxY});
    panelBg.setSize({boxWidth, boxHeight});
}

} // namespace UI::Widgets