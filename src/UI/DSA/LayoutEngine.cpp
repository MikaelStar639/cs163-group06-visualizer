#include "UI/DSA/LayoutEngine.hpp"
#include "UI/Animations/Node/NodeMoveAnimation.hpp" 
#include <cmath>

namespace UI::DSA::LayoutEngine {

    std::unique_ptr<Animations::ParallelAnimation> asLinkedList(
        const Graph& graph, float startX, float startY, float spacing, float duration) 
    {
        auto animGroup = std::make_unique<Animations::ParallelAnimation>();
        const auto& nodes = graph.getNodes();

        for (size_t i = 0; i < nodes.size(); ++i) {
            float targetX = startX + i * spacing;
            float targetY = startY;

            sf::Vector2f currentPos = nodes[i]->getPosition();
            
            //If the node's position is wrong, change it
            if (std::abs(currentPos.x - targetX) > 1.f || std::abs(currentPos.y - targetY) > 1.f) {
                animGroup->add(std::make_unique<Animations::NodeMoveAnimation>(
                    nodes[i].get(), sf::Vector2f(targetX, targetY), duration
                ));
            }
        }

        return animGroup;
    }

    std::unique_ptr<Animations::ParallelAnimation> asHeap(
    const Graph& graph, float startX, float startY, float spacing, float duration) 
    {
        auto animGroup = std::make_unique<Animations::ParallelAnimation>();
        const auto& nodes = graph.getNodes();
        if (nodes.empty()) return animGroup;

        // 1. Determine the total depth of the tree to calculate the "span"
        int maxLevel = static_cast<int>(std::floor(std::log2(nodes.size())));

        for (size_t i = 0; i < nodes.size(); ++i) {
            int level = static_cast<int>(std::floor(std::log2(i + 1)));
            int firstIdxInLevel = static_cast<int>(std::pow(2, level)) - 1;
            int posInLevel = static_cast<int>(i) - firstIdxInLevel;
            int numNodesInLevel = static_cast<int>(std::pow(2, level));

            // 2. The Triangle Logic:
            // The horizontal offset is based on the total width of the bottom level.
            // We want the spacing to halve as we go up, or double as we go down.
            // 'hSpacing' represents the distance between nodes at this specific level.
            float hSpacing = std::pow(2, maxLevel - level) * spacing;
            
            // 3. Center the nodes
            // Total width of this level is (nodes - 1) * spacing
            float levelWidth = (numNodesInLevel - 1) * hSpacing;
            float targetX = startX + (posInLevel * hSpacing) - (levelWidth / 2.0f);
            
            // Vertical spacing can remain constant or be adjusted
            float targetY = startY + (level * spacing * 1.2f); // 1.2f for a slightly taller triangle

            sf::Vector2f currentPos = nodes[i]->getPosition();

            if (std::abs(currentPos.x - targetX) > 1.f || std::abs(currentPos.y - targetY) > 1.f) {
                animGroup->add(std::make_unique<Animations::NodeMoveAnimation>(
                    nodes[i].get(), sf::Vector2f(targetX, targetY), duration
                ));
            }
        }
        return animGroup;
    }

} // namespace UI::DSA::LayoutEngine