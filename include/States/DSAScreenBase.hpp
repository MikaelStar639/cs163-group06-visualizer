#pragma once
#include "States/Screen.hpp"
#include "Core/AppContext.hpp"
#include "UI/DSA/Graph.hpp"

class DSAScreenBase : public Screen {
protected:
    AppContext& ctx;
    UI::DSA::Graph myGraph; 

    //Camera manager
    sf::View graphView;
    bool isPanning = false;
    sf::Vector2i lastPanMousePos;

public:
    explicit DSAScreenBase(AppContext& context, bool directed = true);
    virtual ~DSAScreenBase() = default;
    virtual void handleEvent(const sf::Event& event) override;
    virtual void update(float dt) override;
    virtual void draw() override;
};