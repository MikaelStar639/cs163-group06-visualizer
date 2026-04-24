#pragma once

#include <SFML/Graphics.hpp>
#include "UI/Animations/Core/AnimationManager.hpp"
#include "UI/Animations/StepByStep/StepNavigator.hpp"
#include "States/Screen.hpp"

// AppContext is used to reference multiple items
struct AppContext {
    sf::RenderWindow &window;
    sf::Font         &font;
    
    ScreenState nextState;
    UI::Animations::AnimationManager animManager;
    UI::Animations::StepNavigator stepNavigator;
    bool isStepByStep = true;
};