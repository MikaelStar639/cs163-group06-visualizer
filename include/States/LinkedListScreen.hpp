#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <States/Screen.hpp>
#include <UI/Components/LinkedListMenu.hpp>
#include <UI/DSA/Graph.hpp>
#include <vector>
#include <string>

class LinkedListScreen : public Screen {
private:
    AppContext& ctx;

    // View components
    UI::Widgets::LinkedListMenu uiMenu;
    UI::DSA::Graph myGraph;

    // Helper for handling specialized logic (logging for now)
    void handleMenuAction();

public:
    explicit LinkedListScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};
