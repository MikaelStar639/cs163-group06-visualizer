#pragma once

#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class HeapMenu : public DSAMenuBase {
    public:
        HeapMenu(AppContext& context);
        virtual ~HeapMenu() = default;

        // Visual state management for the main menu buttons
        void setMainButtonEnabled(int index, bool enabled);

    protected:
        // Layout override to handle the larger button widths
        void updateLayout() override;

        // Sub-menu rendering logic based on the menuIndex
        void renderSubMenu(float boxX, float boxY, int menuIndex) override;

        // Provides the labels for the main horizontal menu
        std::vector<std::string> getMainButtonLabels() const override;

        // Determines if an action (like Clear All) triggers immediately
        bool isInstantAction(int index) const override;
    };

} // namespace UI::Widgets