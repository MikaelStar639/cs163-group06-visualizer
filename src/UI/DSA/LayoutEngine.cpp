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

} // namespace UI::DSA::LayoutEngine