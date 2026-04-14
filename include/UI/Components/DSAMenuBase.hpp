#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <UI/Widgets/Button.hpp>
#include <UI/Widgets/InputBar.hpp>
#include <UI/Widgets/Dropdown.hpp>
#include <UI/Widgets/Slider.hpp>
#include <UI/Shapes/RoundedRectangleShape.hpp>
#include <UI/Shapes/PauseIcon.hpp>
#include <UI/Shapes/PlayIcon.hpp>
#include <vector>
#include <string>
#include <optional>

namespace UI::Widgets {

    enum class ActiveMenu {
        None, Create, Insert, Remove, Search, Update, Clean
    };

    class DSAMenuBase {
    protected:
        AppContext& ctx;

        // Common UI Elements
        sf::Text title;
        Button btnBack;
        RoundedRectangleShape panelBg;
        
        // Timeline Controls
        Slider speedSlider;
        Button btnPrev;
        Button btnPlay;
        Button btnNext;
        UI::Shapes::PlayIcon iconPlay;
        UI::Shapes::PauseIcon iconPause;

        // Menu State
        ActiveMenu activeMenu = ActiveMenu::None;
        int lastDropdownIndex = -1;

        // Widget containers
        std::vector<Button> mainButtons;
        std::vector<Button> activeSubButtons;
        std::vector<InputBar> activeInputs;
        std::optional<Dropdown> dropdownAction;

        // Layout helpers
        virtual void updateLayout();
        virtual void renderSubMenu(float boxX, float boxY, ActiveMenu type) = 0;
        virtual std::vector<std::string> getMainButtonLabels() const = 0;

        // Flag for interaction
        bool goClicked = false;
        int clickedSubButtonIndex = -1;

        // Flag for prev button
        bool cancelClicked = false;

    public:
        DSAMenuBase(AppContext& context, const std::string& titleText);
        virtual ~DSAMenuBase() = default;

        virtual void handleEvent(const sf::Event& event);
        virtual void update(sf::Vector2i mousePos);
        virtual void draw(sf::RenderWindow& window);

        // API for Controller (Screen)
        bool isBackClicked(const sf::Event& event) { return btnBack.isClicked(event); }
        bool consumeGoClicked(); // Returns true if Go was clicked, then resets it
        bool consumeCancelClicked();

        ActiveMenu getActiveMenu() const { return activeMenu; }
        int getDropdownSelection() const { return lastDropdownIndex; }
        int getClickedSubButtonIndex() const { return clickedSubButtonIndex; }
        const std::vector<InputBar>& getInputs() const { return activeInputs; }
        
        void resetMenu();
        void clearInputs();
    };

}
