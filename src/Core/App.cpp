#include "Core/App.hpp"

App::App(): font("assets/fonts/SpaceMono.ttf"),
            context{window, font, ScreenState::None} 
{
    sf::ContextSettings settings;
    settings.antiAliasingLevel = 8;
    
    window.create(sf::VideoMode({1600, 900}), 
                  "Visualizer", 
                  sf::Style::Default ^ sf::Style::Resize, 
                  sf::State::Windowed, 
                  settings);

    window.setFramerateLimit(60);
    currentScreen = std::make_unique<TestScreen>(context);
};

void App::run(){
    while (window.isOpen()){
        while (auto event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
                return;
            }
            
            currentScreen->handleEvent(*event);
        }

        currentScreen->update();

        window.clear(Config::Colors::Background); 
        currentScreen->draw();
        window.display();

        if (context.nextState != ScreenState::None){
            changeScreen(context.nextState);
            context.nextState = ScreenState::None;
        }
    }
}

void App::changeScreen(ScreenState nextState) {
    if (nextState == ScreenState::None) return;

    switch (nextState) {
        case ScreenState::MainMenu:
            currentScreen = std::make_unique<MenuScreen>(context);
            break;
        // case ScreenState::LinkedList:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::Heap:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::Trie:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::MST:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;

        case ScreenState::Exit:
            window.close();
            break;

        default:
            break;
    }
}