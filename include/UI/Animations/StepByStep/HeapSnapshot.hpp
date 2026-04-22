#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Core/DSA/Heap.hpp"

namespace UI::DSA { class Node; }

namespace UI::Animations {

    /**
     * @brief Stores a complete visual and logical state of the Heap.
     */
    struct HeapSnapshot {
        // Data Model State
        std::vector<int> pool; 

        // Visual State (Node positions and colors)
        struct NodeState {
            UI::DSA::Node* originalPointer; 
            sf::Vector2f position;
            sf::Color fillColor;
            sf::Color outlineColor;
            sf::Color labelColor;
            float scale;
            std::string label;
        };
        std::vector<NodeState> nodes;

        int activeLineIndex = -1;
    };

} // namespace UI::Animations
