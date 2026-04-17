#pragma once
#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class HeapMenu : public DSAMenuBase {
    public:
        HeapMenu(AppContext& context);
        
        // This is the specific tool the HeapScreen will use to grey out buttons
        void setMainButtonEnabled(int index, bool enabled);

    protected:
        void renderSubMenu(float boxX, float boxY, ActiveMenu type) override;
        std::vector<std::string> getMainButtonLabels() const override;
    };

} // namespace UI::Widgets