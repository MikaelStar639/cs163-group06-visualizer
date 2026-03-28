#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <States/Screen.hpp>
#include <UI/Widgets/Button.hpp>
#include <vector>

class TestScreen : public Screen {
private:
    AppContext& ctx;

public:
    explicit TestScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};