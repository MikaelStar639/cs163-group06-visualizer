#pragma once
#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Core/DSA/LinkedList.hpp"

namespace UI::DSA { class Node; }

namespace UI::Animations {

    /**
     * @brief Stores a complete visual and logical state of the Linked List.
     */
    struct LinkedListSnapshot {
        // Data Model State
        Core::DSA::LinkedList listModel;

        // Visual State (Node positions and colors)
        struct NodeState {
            UI::DSA::Node* originalPointer; // Key for re-identifying the node object
            sf::Vector2f position;
            sf::Color fillColor;
            sf::Color outlineColor;
            sf::Color labelColor;
            float scale;
            std::string label;
        };
        std::vector<NodeState> nodes;

        // PseudoCode State
        int activeLineIndex = -1;
    };

} // namespace UI::Animations
