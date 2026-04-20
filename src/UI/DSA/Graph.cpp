#include "UI/DSA/Graph.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include <algorithm>
#include <iostream>

namespace UI::DSA {

    Graph::Graph(AppContext& context, bool directed)
        : ctx(context), isDirected(directed), isDraggable(true) {}

    void Graph::setDraggable(bool draggable) {
        isDraggable = draggable;
        if (!isDraggable) draggedNode = nullptr;
    }

    bool Graph::getDraggable() const { return isDraggable; }

    void Graph::addNode(const std::string& val, sf::Vector2f pos) {
        nodes.push_back(std::make_unique<Node>(ctx, val, pos));
        
        drawOrder.push_back(nodes.back().get());

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeInsertAnimation>(nodes.back().get(), 0.2f)
        );
    }

    void Graph::insertNodeAt(int index, const std::string& val, sf::Vector2f pos) {
        if (index < 0 || index > nodes.size()) return;

        auto it = nodes.insert(nodes.begin() + index, std::make_unique<Node>(ctx, val, pos));
        
        Node* newNode = it->get();
        drawOrder.push_back(newNode);

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeInsertAnimation>(newNode, 0.2f)
        );
    }

    void Graph::removeNodeAt(int index) {
        if (index < 0 || index >= nodes.size()) return;

        std::unique_ptr<Node> dyingNodePtr = std::move(nodes[index]);
        Node* nodeToDelete = dyingNodePtr.get();
        
        nodes.erase(nodes.begin() + index); 
        dyingNodes.push_back(std::move(dyingNodePtr)); 

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeDeleteAnimation>(
                nodeToDelete, 0.2f, 
                [this, nodeToDelete]() { 
                    edges.erase(
                        std::remove_if(edges.begin(), edges.end(),
                            [nodeToDelete](const std::unique_ptr<Edge>& edge) {
                                return edge->connectsTo(nodeToDelete);
                            }),
                        edges.end()
                    );

                    auto it = std::find(drawOrder.begin(), drawOrder.end(), nodeToDelete);
                    if (it != drawOrder.end()) drawOrder.erase(it);

                    auto dyingIt = std::find_if(dyingNodes.begin(), dyingNodes.end(), 
                        [nodeToDelete](const std::unique_ptr<Node>& n) { return n.get() == nodeToDelete; });
                    
                    if (dyingIt != dyingNodes.end()) dyingNodes.erase(dyingIt);
                }
            )
        );
    }

    void Graph::updateNodeValue(int index, const std::string& newVal){
        if (index < 0 || index >= nodes.size()) return;
    
        Node* targetNode = nodes[index].get();
        targetNode->setLabel(newVal); 

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeHighlightAnimation>(targetNode, 0.3f)
        );
    }

    void Graph::removeLastNode() {
    if (nodes.empty()) return;
    
    removeNodeAt(nodes.size() - 1); 
}

    void Graph::addEdge(int srcIndex, int destIndex, const std::string& weight) {
        if (srcIndex < 0 || srcIndex >= nodes.size() || 
            destIndex < 0 || destIndex >= nodes.size()) return;

        edges.push_back(std::make_unique<Edge>(
            ctx, nodes[srcIndex].get(), nodes[destIndex].get(), isDirected, weight
        ));

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::EdgeInsertAnimation>(edges.back().get(), 0.3f)
        );
    }

    void Graph::removeEdge(int srcIndex, int destIndex) {
        Edge* edgeToDelete = getEdge(srcIndex, destIndex);
        if (!edgeToDelete) return;

        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::EdgeDeleteAnimation>(
                edgeToDelete, 0.2f,
                [this, edgeToDelete]() {
                    edges.erase(
                        std::remove_if(edges.begin(), edges.end(),
                            [edgeToDelete](const std::unique_ptr<Edge>& e) { return e.get() == edgeToDelete; }),
                        edges.end()
                    );
                }
            )
        );
    }

    void Graph::clear() {
        ctx.animManager.clearAll();
        edges.clear();
        nodes.clear();
        drawOrder.clear();
        draggedNode = nullptr;
    }

    void Graph::clearEdges() {
        edges.clear();
    }

    void Graph::resetVisuals(){
        for (auto& node : nodes) {
            node->setFillColor(Config::UI::Colors::NodeFill);
            node->setOutlineColor(Config::UI::Colors::NodeOutline);
            node->setScale(1.f);
            node->setLabelColor(Config::UI::Colors::NodeText);
        }
    }

    void Graph::handleEvent(const sf::Event& event, sf::Vector2f mousePos) {
        if (!isDraggable) return;

        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                for (int i = drawOrder.size() - 1; i >= 0; --i) {
                    auto node = drawOrder[i]; 
                    if (node->contains(mousePos)) {
                        draggedNode = node;
                        dragOffset = node->getPosition() - mousePos;
                        
                        drawOrder.erase(drawOrder.begin() + i);
                        drawOrder.push_back(node);
                        break; 
                    }
                }
            }
        }

        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                draggedNode = nullptr;
            }
        }
    }

    void Graph::update(sf::Vector2f mouseWorldPos) {
        if (draggedNode != nullptr && isDraggable) {
            draggedNode->setPosition(mouseWorldPos + dragOffset);
            draggedNode->onHover(); 
        } else {
            Node* hoveredNode = nullptr;
            for (int i = drawOrder.size() - 1; i >= 0; --i) {
                auto node = drawOrder[i];
                if (node->contains(mouseWorldPos)) {
                    hoveredNode = node; 
                    break; 
                }
            }
            for (const auto &node: drawOrder){
                if (node == hoveredNode){
                    node->onHover();
                } else{
                    node->onIdle();
                }
            }
        }

        for (auto& edge : edges) {
            edge->update();
        }
    }

    void Graph::draw() {
        for (auto& edge : edges) {
            edge->draw();
        }
        for (const auto &node: drawOrder) {
            node->draw();
        }
    }

    const std::vector<std::unique_ptr<Node>>& Graph::getNodes() const { return nodes; }
    const std::vector<std::unique_ptr<Edge>>& Graph::getEdges() const { return edges; }

    bool Graph::isAnimating() const {return !ctx.animManager.empty();}

    Node* Graph::getNode(int index) const {
        if (index >= 0 && index < nodes.size()) return nodes[index].get();
        return nullptr;
    }

    Edge* Graph::getEdge(int srcIndex, int destIndex) const {
        if (srcIndex < 0 || srcIndex >= nodes.size() || destIndex < 0 || destIndex >= nodes.size()) return nullptr;
        Node* src = nodes[srcIndex].get();
        Node* dest = nodes[destIndex].get();
        
        for (const auto& edge : edges) {
            if (getIsDirected()) {
                if (edge->getSource() == src && edge->getDest() == dest) return edge.get();
            } else {
                if (edge->connectsTo(src) && edge->connectsTo(dest)) return edge.get();
            }
        }
        return nullptr;
    }

    sf::FloatRect Graph::getGraphBounds() const {
        if (nodes.empty()) {
            return sf::FloatRect({0.f, 0.f}, {1000.f, 1000.f}); 
        }

        float minX = nodes[0]->getPosition().x;
        float maxX = minX;
        float minY = nodes[0]->getPosition().y;
        float maxY = minY;

        for (const auto& node : nodes) {
            sf::Vector2f pos = node->getPosition();
            minX = std::min(minX, pos.x);
            maxX = std::max(maxX, pos.x);
            minY = std::min(minY, pos.y);
            maxY = std::max(maxY, pos.y);
        }

        return sf::FloatRect({minX, minY}, {maxX - minX, maxY - minY});
    }

    bool Graph::getIsDirected() const  { return isDirected; }


    void Graph::setNodeValueRaw(int index, const std::string& newVal) {
        if (index < 0 || index >= nodes.size()) return;
        nodes[index]->setLabel(newVal);
    }

    void Graph::swapNodePointers(int i, int j) {
        if (i < nodes.size() && j < nodes.size()) {
            std::swap(nodes[i], nodes[j]);
        }
    }

    void Graph::setIsDirected(bool Directed){
        isDirected = Directed;
    }
    
} // namespace UI::DSA