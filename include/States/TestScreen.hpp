#pragma once
#include "States/Screen.hpp"
#include "UI/Widgets/Button.hpp"
#include "UI/DSA/Graph.hpp"
#include <vector>

class TestScreen : public Screen {
private:
    AppContext& ctx;
    UI::Widgets::Button btnInsert;
    UI::DSA::Graph myGraph;

    int draggedNodeIndex = -1; 
    
    sf::Vector2f dragOffset;
    void addNewNode(const std::string &val = "0");

public:
    explicit TestScreen(AppContext& context);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};