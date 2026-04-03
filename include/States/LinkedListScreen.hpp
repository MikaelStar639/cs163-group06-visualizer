#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <States/Screen.hpp>
#include <UI/Widgets/Button.hpp>
#include <UI/Widgets/InputBar.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <UI/Widgets/Dropdown.hpp>
#include <vector>

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

    // Main Left Column Menu
    UI::Widgets::Button btnCreate;
    UI::Widgets::Button btnInsert;
    UI::Widgets::Button btnRemove;
    UI::Widgets::Button btnSearch;
    UI::Widgets::Button btnClean;

    // Right Panel
    RoundedRectangleShape panelBg;
    sf::Text lblParam1;
    sf::Text lblParam2;

    UI::Widgets::InputBar inputParam1;
    UI::Widgets::InputBar inputParam2;

    // Contextual execution buttons
    UI::Widgets::Dropdown dropdownAction;
    UI::Widgets::Button btnExecute;
    int lastDropdownIndex = -1;

    // Timeline Controls
    UI::Widgets::Button btnPrev;
    UI::Widgets::Button btnPlay;
    UI::Widgets::Button btnNext;

    // State
    ActiveMenu activeMenu = ActiveMenu::None;

    // Active Widgets for Generic Update/Draw
    std::vector<UI::Widgets::Button*> mainButtons;
    std::vector<UI::Widgets::Button*> activeSubButtons;
    std::vector<UI::Widgets::InputBar*> activeInputs;
    std::vector<sf::Text*> activeLabels;

    void initUI();
    void updateLayout();
    void renderSubMenu(float boxY, ActiveMenu type);

public:
    explicit LinkedListScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};
