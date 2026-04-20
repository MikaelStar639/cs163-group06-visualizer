#pragma once
#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class TrieMenu : public DSAMenuBase {
    public:
        enum class Action {
            Create = 0, Insert, Remove, Search, Clean
        };

    protected:
        void renderSubMenu(float boxX, float boxY, int menuIndex) override;
        std::vector<std::string> getMainButtonLabels() const override;
        
        bool isInstantAction(int index) const override;

    public:
        explicit TrieMenu(AppContext& context);
    };

} // namespace UI::Widgets