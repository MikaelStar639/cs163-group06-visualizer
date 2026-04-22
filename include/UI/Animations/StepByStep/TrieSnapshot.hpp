#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "Core/DSA/Trie.hpp"

namespace UI::DSA { class Node; }

namespace UI::Animations {

    /**
     * @brief Stores a complete visual and logical state of the Trie.
     */
    struct TrieSnapshot {
        // Data Model State
        Core::DSA::Trie trieModel;
        
        // Logical Mapping
        std::unordered_map<int, int> poolToGraphMap;

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
