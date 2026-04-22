#include "UI/Components/MSTMenu.hpp"

namespace UI::Widgets {

MSTMenu::MSTMenu(AppContext& context)
    : DSAMenuBase(context, "Minimum Spanning Tree")
{
    updateLayout();
}

std::vector<std::string> MSTMenu::getMainButtonLabels() const {
    return {
        "Create",
        "Kruskal",
        "Prim",
        "Clear"
    };
}

bool MSTMenu::isInstantAction(int index) const {
    return index == static_cast<int>(Action::Clear);
}

void MSTMenu::renderSubMenu(float boxX, float boxY, int menuIndex) {
    float innerX = boxX + 15.f;
    float innerY = boxY + 15.f;

    float boxWidth = 0.f;
    float boxHeight = 80.f;

    sf::Color innerBtnIdle  = Config::UI::Colors::ButtonIdle;
    sf::Color innerBtnHover = Config::UI::Colors::ButtonHover;
    sf::Color innerBtnPress = Config::UI::Colors::ButtonPressed;

    auto createDropdown = [&](const std::vector<std::string>& options, float x, float w) {
        dropdownAction.emplace(ctx, "Select...", sf::Vector2f{x, innerY}, sf::Vector2f{w, 45.f});
        dropdownAction->setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
        dropdownAction->setOptions(options);

        if (lastDropdownIndex >= 0 && lastDropdownIndex < static_cast<int>(options.size())) {
            dropdownAction->setSelectedIndex(lastDropdownIndex);
        } else {
            dropdownAction->setSelectedIndex(0);
            lastDropdownIndex = 0;
        }

        dropdownAction->setLabel(dropdownAction->getSelectedText());
        return dropdownAction->getSelectedIndex();
    };

    auto createInput = [&](const std::string& placeholder,
                           float x, float y,
                           float w, float h,
                           InputType inputType = InputType::AnyText) {
        activeInputs.emplace_back(ctx, sf::Vector2f{x, y}, sf::Vector2f{w, h}, "", inputType);
        activeInputs.back().setPlaceholder(placeholder);
    };

    auto createButton = [&](const std::string& label, float x, float y, float w = 90.f, float h = 45.f) {
        activeSubButtons.emplace_back(ctx, label, sf::Vector2f{x, y}, sf::Vector2f{w, h});
        activeSubButtons.back().setColors(innerBtnIdle, innerBtnHover, innerBtnPress, sf::Color::White);
    };

    float currentX = innerX;
    float gap = 15.f;

    Action action = static_cast<Action>(menuIndex);

    if (action == Action::Create) {
        int sel = createDropdown({"Random", "File", "Manual"}, currentX, 170.f);
        currentX += 170.f + gap;

        if (sel == 0) { // Random
            createInput("Nodes", currentX, innerY, 110.f, 45.f, InputType::Integer);
            currentX += 110.f + gap;

            createInput("Edges", currentX, innerY, 110.f, 45.f, InputType::Integer);
            currentX += 110.f + gap;

            createButton("Go", currentX, innerY);
            currentX += 90.f;

            boxHeight = 80.f;
            boxWidth = (currentX - boxX) + 20.f;
        }
        else if (sel == 1) { // File
            createButton("Edit", currentX, innerY);
            currentX += 90.f + gap;

            createButton("Go", currentX, innerY);
            currentX += 90.f;

            boxHeight = 80.f;
            boxWidth = (currentX - boxX) + 20.f;
        }
        else if (sel == 2) { // Manual
            
            createInput("Node Count", currentX, innerY, 65.f, 45.f, InputType::Integer);
            activeInputs.back().setReadOnly(true);
            activeInputs.back().setText("0");

            float windowH = static_cast<float>(ctx.window.getSize().y);

            float graphBoxX = innerX + 10.f;
            float graphBoxY = innerY + 60.f;

            // ô multiline còn 1/2 chiều ngang cũ
            float graphBoxW = 260.f;

            float bottomMargin = 30.f;
            float graphBoxH = std::max(260.f, windowH - graphBoxY - bottomMargin);

            createInput("x or u v w",
                        graphBoxX, graphBoxY, graphBoxW, graphBoxH, InputType::EdgeTriple);


            boxHeight = (graphBoxY + graphBoxH - boxY) + 20.f;
            boxWidth  = (graphBoxX + graphBoxW - boxX) + 20.f;
        }

    }
    else if (action == Action::Kruskal) {
        createButton("Go", currentX, innerY);
        currentX += 90.f;

        boxHeight = 80.f;
        boxWidth = (currentX - boxX) + 20.f;
    }
    else if (action == Action::Prim) {
        createInput("Start", currentX, innerY, 120.f, 45.f, InputType::Integer);
        currentX += 120.f + gap;

        createButton("Go", currentX, innerY);
        currentX += 90.f;

        boxHeight = 80.f;
        boxWidth = (currentX - boxX) + 20.f;
    }

    panelBg.setPosition({boxX, boxY});
    panelBg.setSize({boxWidth, boxHeight});

    float windowWidth = static_cast<float>(ctx.window.getSize().x);
    if (boxX + boxWidth > windowWidth - 20.f) {
        boxX = windowWidth - boxWidth - 20.f;
        panelBg.setPosition({boxX, boxY});
    }
}

} // namespace UI::Widgets