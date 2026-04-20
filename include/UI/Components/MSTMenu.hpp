#pragma once
#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class MSTMenu : public DSAMenuBase {
    public:
        enum class Action {
            Create = 0,
            Kruskal,
            Prim,
            Clean
        };

    protected:
        void renderSubMenu(float boxX, float boxY, int menuIndex) override;
        std::vector<std::string> getMainButtonLabels() const override;
        bool isInstantAction(int index) const override;

    public:
        explicit MSTMenu(AppContext& context);
    };

} // namespace UI::Widgets