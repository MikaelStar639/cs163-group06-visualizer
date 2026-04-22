#include "States/MenuScreen.hpp"
#include "Core/Constants.hpp"

MenuScreen::MenuScreen(AppContext& context):
    ctx(context),      
    btnLinkedList(context, "LINKED LIST",       {625.f, 280.f}),
    btnHeap(context,       "HEAP",              {625.f, 380.f}),
    btnTrie(context,       "TRIE",              {625.f, 480.f}),
    btnMST(context,        "MIN SPANNING TREE", {625.f, 580.f}),
    btnExit(context,       "EXIT",              {625.f, 680.f}), 
    title(context.font,    "DSA VISUALIZER",    70)
{
    initTitle();
    btnExit.setColors(Config::UI::Colors::ButtonIdle, 
                      Config::UI::Colors::ButtonHover, 
                      Config::UI::Colors::ButtonPressed, 
                      sf::Color::Red);
}

void MenuScreen::initTitle() {
    title.setFillColor(sf::Color(173, 216, 230));
    title.setStyle(sf::Text::Bold);

    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin({
        bounds.position.x + bounds.size.x / 2.f,
        bounds.position.y + bounds.size.y / 2.f
    });

    title.setPosition({800.f, 150.f});
}

void MenuScreen::handleEvent(const sf::Event& event) {
    // Change the state
    if (btnLinkedList.isClicked(event)) {
        ctx.nextState = ScreenState::LinkedList;
    } 
    else if (btnHeap.isClicked(event)) {
        ctx.nextState = ScreenState::Heap;
    } 
    else if (btnTrie.isClicked(event)) {
        ctx.nextState = ScreenState::Trie;
    } 
    else if (btnMST.isClicked(event)) {
        ctx.nextState = ScreenState::MST;
    }
    else if (btnExit.isClicked(event)) {
        ctx.nextState = ScreenState::Exit;
    }

    // ESC to Exit
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            ctx.nextState = ScreenState::Exit;
        }
    }
}

//update the buttons
void MenuScreen::update(float dt) {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);

    btnLinkedList.update(mousePos);
    btnHeap.      update(mousePos);
    btnTrie.      update(mousePos);
    btnMST.       update(mousePos);
    btnExit.      update(mousePos);
}

//draw the title and buttons
void MenuScreen::draw() {
    ctx.window.draw(title);
    
    btnLinkedList.draw();
    btnHeap.      draw();
    btnTrie.      draw();
    btnMST.       draw();
    btnExit.      draw();
}