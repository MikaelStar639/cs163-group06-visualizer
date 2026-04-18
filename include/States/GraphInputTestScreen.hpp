#pragma once

#include <SFML/Graphics.hpp>
#include <States/DSAScreenBase.hpp>
#include <Core/AppContext.hpp>
#include <UI/Widgets/InputBar.hpp>
#include <UI/Widgets/Button.hpp>
#include <string>
#include <vector>
#include <tuple>

class GraphInputTestScreen : public DSAScreenBase {
private:
    UI::Widgets::InputBar nodeCountBar;
    UI::Widgets::InputBar graphDataBar;
    UI::Widgets::Button   btnBack;

    sf::Text title;
    sf::Text nodeCountLabel;
    sf::Text graphDataLabel;
    sf::Text hintText;
    sf::Text statusText;

    sf::RectangleShape previewBox;

    void rebuildGraphFromInputs();
    void clearGraphWithStatus(const std::string& msg, sf::Color color = sf::Color::Red);

public:
    explicit GraphInputTestScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};