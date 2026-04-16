#include "States/GraphInputTestScreen.hpp"
#include <cmath>
#include <sstream>

#include "States/GraphInputTestScreen.hpp"
#include <cmath>
#include <sstream>

GraphInputTestScreen::GraphInputTestScreen(AppContext& context)
    : DSAScreenBase(context),
      nodeCountBar(context,
                   {50.f, 95.f},
                   {90.f, 45.f},
                   "N",
                   UI::Widgets::InputType::Integer),
      graphDataBar(context,
                   {50.f, 200.f},
                   {420.f, 520.f},
                   "Each line: x    or    u v w",
                   UI::Widgets::InputType::EdgeTriple),
      btnBack(context, "Back", {50.f, 740.f}, {120.f, 50.f}),
      title(context.font, "GRAPH INPUT TEST", 34),
      nodeCountLabel(context.font, "Node Count:", 26),
      graphDataLabel(context.font, "Graph Data:", 26),
      hintText(context.font, "", 20),
      statusText(context.font, "", 20)
{
    myGraph.setDraggable(false);

    title.setFillColor(sf::Color::White);
    title.setPosition({50.f, 20.f});

    nodeCountLabel.setFillColor(sf::Color::White);
    nodeCountLabel.setPosition({50.f, 65.f});

    graphDataLabel.setFillColor(sf::Color::White);
    graphDataLabel.setPosition({50.f, 165.f});

    hintText.setFillColor(sf::Color(220, 220, 220));
    hintText.setString(
        "Format:\n"
        "- First N lines: node labels\n"
        "- Remaining lines: edges in form u v w\n"
        "Example:\n"
        "0\n1\n2\n3\n4\n5\n"
        "0 2 8\n0 4 7\n0 5 6\n1 4 5\n1 5 4\n2 3 3\n2 4 2\n4 5 1"
    );
    hintText.setPosition({50.f, 810.f});

    statusText.setFillColor(sf::Color::Green);
    statusText.setPosition({50.f, 725.f});

    previewBox.setPosition({540.f, 50.f});
    previewBox.setSize({580.f, 670.f});
    previewBox.setFillColor(sf::Color::Transparent);
    previewBox.setOutlineThickness(2.f);
    previewBox.setOutlineColor(sf::Color(200, 200, 200));

    nodeCountBar.setText("6");
    graphDataBar.setText(
        "0\n"
        "1\n"
        "2\n"
        "3\n"
        "4\n"
        "5\n"
        "0 2 8\n"
        "0 4 7\n"
        "0 5 6\n"
        "1 4 5\n"
        "1 5 4\n"
        "2 3 3\n"
        "2 4 2\n"
        "4 5 1"
    );

    rebuildGraphFromInputs();
}

void GraphInputTestScreen::clearGraphWithStatus(const std::string& msg, sf::Color color) {
    myGraph.clear();
    statusText.setString(msg);
    statusText.setFillColor(color);
}

void GraphInputTestScreen::rebuildGraphFromInputs() {
    using std::tuple;
    using std::vector;
    using std::string;

    if (!nodeCountBar.valid()) {
        clearGraphWithStatus("Invalid node count");
        return;
    }

    if (nodeCountBar.empty()) {
        clearGraphWithStatus("Enter node count");
        return;
    }

    int n = 0;
    try {
        n = std::stoi(nodeCountBar.getText());
    } catch (...) {
        clearGraphWithStatus("Node count must be an integer");
        return;
    }

    if (n <= 0) {
        clearGraphWithStatus("Node count must be > 0");
        return;
    }

    if (!graphDataBar.valid()) {
        clearGraphWithStatus(graphDataBar.getErrorMessage());
        return;
    }

    vector<int> nodeValues;
    vector<tuple<int, int, int>> edges;
    string err;

    if (!graphDataBar.parseGraphData(n, nodeValues, edges, err)) {
        clearGraphWithStatus(err);
        return;
    }

    for (const auto& [u, v, w] : edges) {
        if (u < 0 || u >= n || v < 0 || v >= n) {
            clearGraphWithStatus("Edge index out of range");
            return;
        }
    }

    myGraph.clear();

    // bố trí node trong khung preview
    const sf::Vector2f boxPos  = previewBox.getPosition();
    const sf::Vector2f boxSize = previewBox.getSize();

    sf::Vector2f center(boxPos.x + boxSize.x / 2.f,
                        boxPos.y + boxSize.y / 2.f);

    float radius = std::min(boxSize.x, boxSize.y) * 0.32f;

    if (n == 1) {
        myGraph.addNode(std::to_string(nodeValues[0]), center);
    } else {
        for (int i = 0; i < n; ++i) {
            float angle = -3.14159265f / 2.f + i * 2.f * 3.14159265f / n;
            sf::Vector2f pos(
                center.x + radius * std::cos(angle),
                center.y + radius * std::sin(angle)
            );
            myGraph.addNode(std::to_string(nodeValues[i]), pos);
        }
    }

    for (const auto& [u, v, w] : edges) {
        myGraph.addEdge(u, v, std::to_string(w));
    }

    statusText.setString("Graph updated");
    statusText.setFillColor(sf::Color::Green);
}

void GraphInputTestScreen::handleEvent(const sf::Event& event) {
    nodeCountBar.handleEvent(event);
    graphDataBar.handleEvent(event);

    DSAScreenBase::handleEvent(event);

    if (btnBack.isClicked(event)) {
        ctx.nextState = ScreenState::MainMenu;
    }

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            ctx.nextState = ScreenState::MainMenu;
        }
    }
}

void GraphInputTestScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);

    btnBack.update(mousePos);
    nodeCountBar.update();
    graphDataBar.update();

    if (nodeCountBar.consumeChanged() || graphDataBar.consumeChanged()) {
        rebuildGraphFromInputs();
    }

    DSAScreenBase::update();
}

void GraphInputTestScreen::draw() {
    DSAScreenBase::draw();

    ctx.window.setView(ctx.window.getDefaultView());

    ctx.window.draw(title);
    ctx.window.draw(nodeCountLabel);
    ctx.window.draw(graphDataLabel);
    ctx.window.draw(hintText);
    ctx.window.draw(statusText);
    ctx.window.draw(previewBox);

    nodeCountBar.draw();
    graphDataBar.draw();
    btnBack.draw();
}