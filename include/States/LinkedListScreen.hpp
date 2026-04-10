#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <Core/DSA/LinkedList.hpp>
#include <States/Screen.hpp>
#include <States/DSAScreenBase.hpp>
#include <UI/Components/LinkedListMenu.hpp>
#include <UI/DSA/Graph.hpp>
#include <vector>
#include <string>
#include "Controllers/LinkedListController.hpp"

class LinkedListScreen : public DSAScreenBase {
private:
    UI::Widgets::LinkedListMenu uiMenu;
    Core::DSA::LinkedList model;
    Controllers::LinkedListController controller;

    void handleMenuAction();

public:
    explicit LinkedListScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};
