#pragma once
#include <SFML/Graphics.hpp>

namespace Config {
    namespace UI {
        //Radius
        namespace Radius {
            inline constexpr float None = 0.f;
            inline constexpr float Sm   = 4.f;
            inline constexpr float Md   = 8.f;
            inline constexpr float Lg   = 12.f;
            inline constexpr float Xl   = 16.f;
            inline constexpr float Full = 9999.f;
        }

        //Default rounded rect shape
        inline constexpr float DEFAULT_CORNER_RADIUS = Radius::Md;
        inline constexpr std::size_t CORNER_POINTS   = 15;
        inline constexpr float OUTLINE_THICKNESS     = 2.f;
        
        // Font size
        inline constexpr unsigned int FONT_SIZE_BUTTON = 24;
        inline constexpr unsigned int FONT_SIZE_NODE   = 24;
    
        //Button
        inline constexpr float BUTTON_WIDTH         = 400.f;
        inline constexpr float BUTTON_HEIGHT        = 70.f;
        inline constexpr float BUTTON_OUTLINE       = 2.f;
        inline constexpr float BUTTON_CORNER_RADIUS = Radius::Lg;

        //Input Bar
        inline constexpr float INPUT_BAR_WIDTH      = 300.f;
        inline constexpr float INPUT_BAR_HEIGHT     = 60.f;

        
        //Nodes
        inline constexpr float NODE_RADIUS = 40.f;
        inline constexpr float NODE_OUTLINE_THICKNESS = 3.f;
        inline constexpr std::size_t NODE_POINT_COUNT = 30;

        //Edges
        inline constexpr float EDGE_THICKNESS = 2.0f;

        namespace Colors {
            //Buttons
            const sf::Color ButtonIdle         = sf::Color(60, 60, 60);  
            const sf::Color ButtonHover        = sf::Color(85, 85, 85);  
            const sf::Color ButtonPressed      = sf::Color(120, 120, 120); 
            const sf::Color ButtonText         = sf::Color::White;
            const sf::Color ButtonOutline      = sf::Color(100, 100, 100); 
            const sf::Color ButtonOutlineHover = sf::Color(173, 216, 230);
        
            //Nodes
            const sf::Color NodeFill    = sf::Color(45, 45, 45); 
            const sf::Color NodeOutline = sf::Color(100, 100, 100); 
            const sf::Color NodeText    = sf::Color::White;
            const sf::Color NodeHighlight     = sf::Color(255, 165, 0); 
            const sf::Color LabelHighlight    = sf::Color(30, 30, 30);
            
            //Edges
            const sf::Color EdgeHighlight     = sf::Color(255, 165, 0); 
            const sf::Color EdgeFill = sf::Color(150, 150, 150);
            
            //UI States
            const sf::Color NodeHover   = sf::Color(173, 216, 230);

            //Input Bars
            const sf::Color InputBarIdle            = sf::Color(45, 45, 45);
            const sf::Color InputBarFocuse          = sf::Color(70, 70, 70);
            const sf::Color InputBarOutline         = sf::Color(100, 100, 100);
            const sf::Color InputBarFocusedOutline  = sf::Color(173, 216, 230);
            const sf::Color InputBarText            = sf::Color::White;
            const sf::Color InputBarPlaceholder     = sf::Color(150, 150, 150);


        }
    }

    namespace Colors {
        const sf::Color Background = sf::Color(30, 30, 30);
    }
}