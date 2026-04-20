#pragma once
#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class LinkedListMenu : public DSAMenuBase {
    public:
        enum class Action {
            Create = 0, Insert, Delete, Search, Update, Clean
        };

    protected:
        void renderSubMenu(float boxX, float boxY, int menuIndex) override;
        std::vector<std::string> getMainButtonLabels() const override;
        bool isInstantAction(int index) const override;

    public:
        LinkedListMenu(AppContext& context);
    };

}