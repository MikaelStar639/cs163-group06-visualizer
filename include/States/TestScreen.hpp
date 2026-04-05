#pragma once
#include "States/Screen.hpp"
#include "UI/Widgets/Button.hpp"
#include "UI/DSA/Node.hpp"
#include "UI/DSA/Edge.hpp"  // Added this
#include <vector>

class TestScreen : public Screen {
private:
    AppContext& ctx;
    UI::Widgets::Button btnInsert;
    std::vector<std::unique_ptr<UI::DSA::Node>> nodes;
    std::vector<std::unique_ptr<UI::DSA::Edge>> edges;
    std::vector<int> drawOrder;

    int draggedNodeIndex = -1; 
    
    sf::Vector2f dragOffset;
    void addNewNode(const std::string &val = "0");

public:
    explicit TestScreen(AppContext& context);
    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};