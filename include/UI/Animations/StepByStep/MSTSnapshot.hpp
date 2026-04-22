#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

namespace UI::Animations {

    struct MSTSnapshot {
        struct NodeState {
            sf::Vector2f position;
            sf::Color fillColor;
            sf::Color labelColor;
            float scale;
        };
        struct EdgeState {
            sf::Color color;
            float thickness;
        };
        std::vector<NodeState> nodes;
        std::vector<EdgeState> edges;
        std::string liveMessage;
        int liveTotalWeight;
        int liveSelectedEdgeCount;
        bool running;
        int activeLineIndex = -1;
    };

} // namespace UI::Animations
