#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <Core/DSA/MST.hpp>
#include <States/Screen.hpp>
#include <States/DSAScreenBase.hpp>
#include <UI/Components/MSTMenu.hpp>
#include <UI/Widgets/PseudoCodeViewer.hpp>
#include <UI/DSA/Graph.hpp>
#include <vector>
#include <string>
#include "Controllers/MSTController.hpp"

class MSTScreen : public DSAScreenBase {
private:
    UI::Widgets::MSTMenu uiMenu;
    UI::Widgets::PseudoCodeViewer codeViewer;
    Core::DSA::MST model;
    Controllers::MSTController controller;

    void handleMenuAction();    
    sf::Text statusText;

    void refreshStatusText();

    bool hasPreviewCache = false;
    std::vector<int> lastPreviewNodeValues;
    std::vector<std::tuple<int,int,int>> lastPreviewEdges;

    void updateManualPreview();
    void clearManualPreviewCache();

    sf::Text liveText;

    void syncGraphToManualCache();
    std::string buildManualTextFromModel() const;

public:
    explicit MSTScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw() override;
};