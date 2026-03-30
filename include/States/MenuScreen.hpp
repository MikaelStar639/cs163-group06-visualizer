#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <States/Screen.hpp>
#include <UI/Widgets/Button.hpp>
#include <vector>

class MenuScreen : public Screen {
private:
    AppContext& ctx;

    sf::Text title;
    
    //Buttons
    UI::Widgets::Button btnLinkedList;
    UI::Widgets::Button btnHeap;
    UI::Widgets::Button btnTrie;
    UI::Widgets::Button btnMST;
    UI::Widgets::Button btnExit;

    //used in construction
    void initTitle();

public:
    explicit MenuScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};