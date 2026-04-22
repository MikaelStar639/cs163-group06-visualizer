#pragma once

#include <SFML/Graphics.hpp>

// all states of screen
enum ScreenState {
    None,
    MainMenu,
    LinkedList,
    Heap,
    Trie,
    MST,
    Settings,
    Exit
};

// abstract class for screen
class Screen {
public:
    virtual ~Screen() = default;
    virtual void handleEvent(const sf::Event& event) = 0;
    virtual void update(float dt) = 0;
    virtual void draw() = 0;
};