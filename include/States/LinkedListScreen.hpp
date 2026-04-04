#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <States/Screen.hpp>
#include <UI/Widgets/Button.hpp>
#include <UI/Widgets/InputBar.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <UI/Widgets/Dropdown.hpp>
#include <vector>
#include <memory>

// Enum for State Management
enum class ActiveMenu {
    None, Create, Insert, Remove, Search, Clean
};

class LinkedListScreen : public Screen {
private:
    AppContext& ctx;

    // Header 
    sf::Text title;
    UI::Widgets::Button btnBack;

    // Right Panel
    RoundedRectangleShape panelBg;

    // Contextual execution
    std::unique_ptr<UI::Widgets::Dropdown> dropdownAction;
    int lastDropdownIndex = -1;

    // Timeline Controls
    UI::Widgets::Button btnPrev;
    UI::Widgets::Button btnPlay;
    UI::Widgets::Button btnNext;

    // State
    ActiveMenu activeMenu = ActiveMenu::None;

    // Active Widgets managed by unique_ptr
    std::vector<std::unique_ptr<UI::Widgets::Button>> mainButtons;
    std::vector<std::unique_ptr<UI::Widgets::Button>> activeSubButtons;
    std::vector<std::unique_ptr<UI::Widgets::InputBar>> activeInputs;

    void initUI();
    void updateLayout();
    void renderSubMenu(float boxX, float boxY, ActiveMenu type);

public:
    explicit LinkedListScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};
