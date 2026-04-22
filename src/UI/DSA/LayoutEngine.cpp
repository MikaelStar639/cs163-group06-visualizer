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
        const Graph& graph, float startX, float startY, float spacing, float duration, float vSpacing) 
    {
        auto animGroup = std::make_unique<Animations::ParallelAnimation>();
        const auto& nodes = graph.getNodes();
        if (nodes.empty()) return animGroup;

        int n = static_cast<int>(nodes.size());
        int maxLevel = static_cast<int>(std::floor(std::log2(n)));

        for (size_t i = 0; i < nodes.size(); ++i) {
            int level = static_cast<int>(std::floor(std::log2(i + 1)));
            
            float targetX = startX;
            int tempIdx = static_cast<int>(i);
            for (int l = level; l > 0; --l) {
                int parentIdx = (tempIdx - 1) / 2;
                bool isRightChild = (tempIdx % 2 == 0);
                float hOffset = std::pow(2.f, maxLevel - l) * (spacing / 2.f);
                targetX += (isRightChild ? 1 : -1) * hOffset;
                tempIdx = parentIdx;
            }
            
            float targetY = startY + (level * spacing * 0.8f); 

            sf::Vector2f currentPos = nodes[i]->getPosition();
            if (std::abs(currentPos.x - targetX) > 0.5f || std::abs(currentPos.y - targetY) > 0.5f) {
                animGroup->add(std::make_unique<Animations::NodeMoveAnimation>(
                    nodes[i].get(), sf::Vector2f(targetX, targetY), duration
                ));
            }
        }
        return animGroup;
    }

    std::unique_ptr<Animations::ParallelAnimation> asTrie(
        const Graph& graph, const Core::DSA::Trie& model, 
        const std::unordered_map<int, int>& poolToGraphMap,
        float startX, float startY, float hSpacing, float vSpacing, float duration) 
    {
        auto animGroup = std::make_unique<Animations::ParallelAnimation>();
        std::unordered_map<int, sf::Vector2f> targetPositions;
        
        const auto& pool = model.getPool();
        int rootIdx = model.getRootIndex();
        float currentLeafX = 0.f; 
        
        auto calculateDFS = [&](auto& self, int poolIdx, int depth) -> float {
            std::vector<int> children;
            for (int i = 0; i < 26; ++i) {
                if (pool[poolIdx].children[i] != -1) children.push_back(pool[poolIdx].children[i]);
            }
            
            float myX = 0.f;
            if (children.empty()) { 
                myX = currentLeafX;
                currentLeafX += hSpacing;
            } else { 
                float sumX = 0;
                for (int child : children) sumX += self(self, child, depth + 1);
                myX = sumX / children.size();
            }
            targetPositions[poolIdx] = {myX, startY + depth * vSpacing};
            return myX;
        };
        
        if (rootIdx != -1) {
            calculateDFS(calculateDFS, rootIdx, 0);
            
            float offsetX = startX - targetPositions[rootIdx].x;
            
            for (auto& pair : targetPositions) {
                pair.second.x += offsetX;
                
                auto it = poolToGraphMap.find(pair.first);
                if (it != poolToGraphMap.end()) {
                    Node* node = graph.getNode(it->second);
                    if (node) {
                        sf::Vector2f currPos = node->getPosition();
                        if (std::abs(currPos.x - pair.second.x) > 1.f || std::abs(currPos.y - pair.second.y) > 1.f) {
                            animGroup->add(std::make_unique<Animations::NodeMoveAnimation>(node, pair.second, duration));
                        }
                    }
                }
            }
        }
        return animGroup;
    }
} // namespace UI::DSA::LayoutEngine