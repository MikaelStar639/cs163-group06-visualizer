#pragma once
#include "UI/Components/DSAMenuBase.hpp"

namespace UI::Widgets {

    class TrieMenu : public DSAMenuBase {
    public:
        enum class Action {
            Create = 0, Insert, Remove, Search, Clean
        };

    protected:
        // Đổi ActiveMenu type thành int menuIndex
        void renderSubMenu(float boxX, float boxY, int menuIndex) override;
        std::vector<std::string> getMainButtonLabels() const override;
        
        // Thêm hàm xác định nút bấm "phát ăn ngay"
        bool isInstantAction(int index) const override;

    public:
        explicit TrieMenu(AppContext& context);
    };

} // namespace UI::Widgets