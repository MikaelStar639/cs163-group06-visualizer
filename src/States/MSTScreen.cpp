#include "States/MSTScreen.hpp"
#include <iostream>

MSTScreen::MSTScreen(AppContext& context)
    : DSAScreenBase(context, false),   
      uiMenu(context),
      codeViewer(context.font),
      controller(context, myGraph, model, &codeViewer),
      liveText(context.font, "", 20),
      statusText(context.font, "", 22)
      
{
    myGraph.setDraggable(true);

    sf::Vector2u winSize = context.window.getSize();
    codeViewer.setPositionBottomRight(static_cast<float>(winSize.x),
                                      static_cast<float>(winSize.y),
                                      50.f);

    statusText.setFillColor(sf::Color(220, 220, 220));
    statusText.setPosition({180.f, 850.f});
    liveText.setFillColor(sf::Color(180, 220, 255));
    liveText.setPosition({1000.f, 30.f});
    refreshStatusText();
}

void MSTScreen::handleEvent(const sf::Event& event) {
    uiMenu.handleEvent(event);
    if (!(uiMenu.getActiveMenuIndex() == static_cast<int>(UI::Widgets::MSTMenu::Action::Create) &&
                                                          uiMenu.getDropdownSelection() == 2)) {
        clearManualPreviewCache();
    }
    DSAScreenBase::handleEvent(event);

    if (uiMenu.isBackClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    if (uiMenu.consumeGoClicked()) {
        if (myGraph.isAnimating()) {
            std::cout << "[WARNING] Wait!\n";
            return;
        }

        handleMenuAction();

        bool shouldClearInputs = true;
        if (uiMenu.getActiveMenuIndex() == static_cast<int>(UI::Widgets::MSTMenu::Action::Create) &&
            uiMenu.getDropdownSelection() == 2) {
            shouldClearInputs = false;
        }

        if (shouldClearInputs) {
            uiMenu.clearInputs();
        }

        if (uiMenu.getActiveMenuIndex() == static_cast<int>(UI::Widgets::MSTMenu::Action::Clear)) {
            uiMenu.resetMenu();
        }
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            ctx.nextState = ScreenState::MainMenu;
        }
    }
}

void MSTScreen::handleMenuAction() {
    using namespace UI::Widgets;

    int menuIndex = uiMenu.getActiveMenuIndex();
    if (menuIndex == -1) return;

    MSTMenu::Action action = static_cast<MSTMenu::Action>(menuIndex);

    int sel = uiMenu.getDropdownSelection();
    const auto& inputs = uiMenu.getInputs();

    if (action == MSTMenu::Action::Create) {
        if (sel == 0) { // Random
            if (inputs.size() < 2) return;
            if (inputs[0].getText().empty() || inputs[1].getText().empty()) return;

            int nodeCount = std::stoi(inputs[0].getText());
            int edgeCount = std::stoi(inputs[1].getText());

            controller.handleCreateRandom(nodeCount, edgeCount);
            syncGraphToManualCache();
            clearManualPreviewCache();
        }
        else if (sel == 1) { // File
            int subBtn = uiMenu.getClickedSubButtonIndex();
            if (subBtn == 0) {
                controller.handleEditDataFile();
            } else if (subBtn == 1) {
                controller.handleCreateFromFile();
                syncGraphToManualCache();
                clearManualPreviewCache();
            }
        }
        else if (sel == 2) { // Manual
            if (inputs.size() < 2) return;

            std::vector<int> nodeValues;
            std::vector<std::tuple<int,int,int>> rawEdges;
            std::string err;

            if (!inputs[1].parseAutoGraphData(nodeValues, rawEdges, err)) {
                std::cout << "[UI LOG] Manual MST parse failed: " << err << '\n';
                return;
            }

            controller.handleCreateManual(static_cast<int>(nodeValues.size()),
                                        nodeValues,
                                        rawEdges);
            
            syncGraphToManualCache();

            lastPreviewNodeValues = nodeValues;
            lastPreviewEdges = rawEdges;
            hasPreviewCache = true;
        }
    }
    else if (action == MSTMenu::Action::Kruskal) {
        controller.handleRunKruskal();
    }
    else if (action == MSTMenu::Action::Prim) {
        if (inputs.empty() || inputs[0].getText().empty()) return;

        int startNode = std::stoi(inputs[0].getText());
        controller.handleRunPrim(startNode);
    }
    else if (action == MSTMenu::Action::Clear) {
        controller.handleClearAll();
        syncGraphToManualCache();
        clearManualPreviewCache();
    }
}

void MSTScreen::update(float dt) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
    uiMenu.update(mousePos, dt);
    
    if (uiMenu.consumeCancelClicked()) {
        ctx.animManager.clearAll();
        controller.forceSnapLayout(); 
        codeViewer.hide();
    }

    updateManualPreview();

    refreshStatusText();

    DSAScreenBase::update(dt);
}
void MSTScreen::draw() {
    DSAScreenBase::draw();
    codeViewer.draw(ctx.window);
    uiMenu.draw(ctx.window);
    ctx.window.draw(statusText);
    ctx.window.draw(liveText);
}

void MSTScreen::refreshStatusText() {
    int shownEdgeCount = controller.isRunning()
        ? controller.getLiveSelectedEdgeCount()
        : controller.getLastSelectedEdgeCount();

    int shownWeight = controller.isRunning()
        ? controller.getLiveTotalWeight()
        : controller.getLastTotalWeight();

    std::string text =
        "Algorithm: " + controller.getLastAlgorithm() +
        "    |    Nodes: " + std::to_string(model.getNodeCount()) +
        "    |    Edges: " + std::to_string(model.getEdges().size()) +
        "    |    MST Edges: " + std::to_string(shownEdgeCount) +
        "    |    Total Weight: " + std::to_string(shownWeight);

    statusText.setString(text);
    liveText.setString("Status: " + controller.getLiveMessage());
}

void MSTScreen::clearManualPreviewCache() {
    hasPreviewCache = false;
    lastPreviewNodeValues.clear();
    lastPreviewEdges.clear();
}

void MSTScreen::updateManualPreview() {
    if (uiMenu.getActiveMenuIndex() != static_cast<int>(UI::Widgets::MSTMenu::Action::Create) ||
        uiMenu.getDropdownSelection() != 2) {
        return;
    }

    auto& inputs = uiMenu.getInputsMutable();
    if (inputs.size() < 2) return;

    // inputs[0] = node count display
    // inputs[1] = graph data multiline
    if (!inputs[1].consumeChanged()) return;

    if (!ctx.animManager.empty()) {
        ctx.animManager.clearAll();
        myGraph.resetVisuals();
        controller.interruptRunning("Graph edited - animation cancelled");
    }

    std::vector<int> nodeValues;
    std::vector<std::tuple<int,int,int>> rawEdges;
    std::string err;

    if (!inputs[1].parseAutoGraphData(nodeValues, rawEdges, err)) {
        if (hasPreviewCache) {
            inputs[0].setText(std::to_string(lastPreviewNodeValues.size()));
        } else {
            inputs[0].setText("0");
        }
        return;
    }

    inputs[0].setText(std::to_string(nodeValues.size()));

    bool changed = (!hasPreviewCache) ||
                   (nodeValues != lastPreviewNodeValues) ||
                   (rawEdges != lastPreviewEdges);

    if (!changed) return;

    controller.handleCreateManual(static_cast<int>(nodeValues.size()),
                                  nodeValues,
                                  rawEdges);

    lastPreviewNodeValues = nodeValues;
    lastPreviewEdges = rawEdges;
    hasPreviewCache = true;
}


std::string MSTScreen::buildManualTextFromModel() const {
    std::string result;

    const auto& nodeValues = model.getNodeValues();
    const auto& edges = model.getEdges();

    // node labels
    for (int val : nodeValues) {
        result += std::to_string(val) + "\n";
    }

    for (const auto& e : edges) {
        if (e.u >= 0 && e.u < static_cast<int>(nodeValues.size()) &&
            e.v >= 0 && e.v < static_cast<int>(nodeValues.size())) {
            result += std::to_string(nodeValues[e.u]) + " "
                   +  std::to_string(nodeValues[e.v]) + " "
                   +  std::to_string(e.w) + "\n";
        }
    }

    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }

    return result;
}

void MSTScreen::syncGraphToManualCache() {
    int createMenuIndex = static_cast<int>(UI::Widgets::MSTMenu::Action::Create);
    int manualDropdownIndex = 2; // Random=0, File=1, Manual=2

    std::vector<std::string> values;
    values.push_back(std::to_string(model.getNodeCount())); 
    values.push_back(buildManualTextFromModel());           

    uiMenu.setCachedInputsForState(createMenuIndex, manualDropdownIndex, values);
}