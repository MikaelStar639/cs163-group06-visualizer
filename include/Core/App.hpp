#pragma once

#include <memory>
#include <SFML/Graphics.hpp>
#include "Core/AppContext.hpp"
#include "States/Screen.hpp"


class App{
private:
    sf::RenderWindow window;
    sf::Font font;
    sf::Image icon;

    //used for reference
    AppContext context;

    //a pointer points to a Screen
    std::unique_ptr<Screen> currentScreen;

    //to change screen state
    void changeScreen(ScreenState nextScreen);

public:
    App();
    void run();
};