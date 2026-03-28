#include "States/TestScreen.hpp"
#include "Core/Constants.hpp"

TestScreen::TestScreen(AppContext& context):
    ctx(context){

}

void TestScreen::handleEvent(const sf::Event& event) {

    // ESC to Exit
    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Escape) {
            ctx.nextState = ScreenState::Exit;
        }
    }
}

//update the buttons
void TestScreen::update() {
    sf::Vector2i mousePos = sf::Mouse::getPosition(ctx.window);
}

//draw the title and buttons
void TestScreen::draw() {
    
}