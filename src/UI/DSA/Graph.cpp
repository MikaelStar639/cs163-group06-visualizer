#include "UI/DSA/Graph.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Node/NodeColorAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include <algorithm>
#include <iostream>
#include <cmath>

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

    Node* Graph::addNodeRaw(const std::string& val, sf::Vector2f pos) {
        nodes.push_back(std::make_unique<Node>(ctx, val, pos));
        Node* newNode = nodes.back().get();
        drawOrder.push_back(newNode);
        return newNode;
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

    std::unique_ptr<Node> Graph::extractNode(int index) {
        if (index < 0 || index >= nodes.size()) return nullptr;
        
        std::unique_ptr<Node> node = std::move(nodes[index]);
        Node* ptr = node.get();
        
        nodes.erase(nodes.begin() + index);
        
        auto it = std::find(drawOrder.begin(), drawOrder.end(), ptr);
        if (it != drawOrder.end()) drawOrder.erase(it);
        
        velocities.erase(ptr);
        lockedNodes.erase(ptr);
        
        return node;
    }

    void Graph::insertNodePtr(int index, std::unique_ptr<Node> node) {
        if (!node) return;
        Node* ptr = node.get();
        
        if (index < 0 || index > nodes.size()) index = nodes.size();
        nodes.insert(nodes.begin() + index, std::move(node));
        
        drawOrder.push_back(ptr);
    }

    void Graph::removeNodeAt(int index) {
        if (index < 0 || index >= nodes.size()) return;

        std::unique_ptr<Node> dyingNodePtr = std::move(nodes[index]);
        Node* nodeToDelete = dyingNodePtr.get();
        
        // Dọn dẹp bộ nhớ vật lý và khóa
        velocities.erase(nodeToDelete);
        lockedNodes.erase(nodeToDelete);

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

    void Graph::addEdge(int srcIndex, int destIndex, const std::string& weight, bool animate) {
        if (srcIndex < 0 || srcIndex >= nodes.size() || 
            destIndex < 0 || destIndex >= nodes.size()) return;

        edges.push_back(std::make_unique<Edge>(
            ctx, nodes[srcIndex].get(), nodes[destIndex].get(), isDirected, weight
        ));

        if (animate) {
            ctx.animManager.addAnimation(
                std::make_unique<UI::Animations::EdgeInsertAnimation>(edges.back().get(), 0.3f)
            );
        }
    }

    void Graph::removeEdge(int srcIndex, int destIndex) {
        Edge* edgeToDelete = getEdge(srcIndex, destIndex);
        if (!edgeToDelete) return;

        auto it = std::find_if(edges.begin(), edges.end(),
            [edgeToDelete](const std::unique_ptr<Edge>& e) { return e.get() == edgeToDelete; });
        
        if (it != edges.end()) {
            std::unique_ptr<Edge> dyingEdgePtr = std::move(*it);
            edges.erase(it);
            dyingEdges.push_back(std::move(dyingEdgePtr));

            ctx.animManager.addAnimation(
                std::make_unique<UI::Animations::EdgeDeleteAnimation>(
                    edgeToDelete, 0.2f,
                    [this, edgeToDelete]() {
                        auto dyingIt = std::find_if(dyingEdges.begin(), dyingEdges.end(),
                            [edgeToDelete](const std::unique_ptr<Edge>& e) { return e.get() == edgeToDelete; });
                        if (dyingIt != dyingEdges.end()) dyingEdges.erase(dyingIt);
                    }
                )
            );
        }
    }

    void Graph::removeEdgeAt(int index, bool animate) {
        if (index < 0 || index >= edges.size()) return;

        if (animate) {
            std::unique_ptr<Edge> dyingEdgePtr = std::move(edges[index]);
            Edge* edgeToDelete = dyingEdgePtr.get();
            edges.erase(edges.begin() + index);
            dyingEdges.push_back(std::move(dyingEdgePtr));

            ctx.animManager.addAnimation(
                std::make_unique<UI::Animations::EdgeDeleteAnimation>(
                    edgeToDelete, 0.2f,
                    [this, edgeToDelete]() {
                        auto dyingIt = std::find_if(dyingEdges.begin(), dyingEdges.end(),
                            [edgeToDelete](const std::unique_ptr<Edge>& e) { return e.get() == edgeToDelete; });
                        if (dyingIt != dyingEdges.end()) dyingEdges.erase(dyingIt);
                    }
                )
            );
        } else {
            edges.erase(edges.begin() + index);
        }
    }

    void Graph::clear() {
        ctx.animManager.clearAll();
        edges.clear();
        dyingEdges.clear();
        nodes.clear();
        dyingNodes.clear();
        drawOrder.clear();
        velocities.clear(); 
        lockedNodes.clear(); 
        draggedNode = nullptr;
    }

    void Graph::clearEdges() {
        while (!edges.empty()) {
            std::unique_ptr<Edge> dyingEdgePtr = std::move(edges.back());
            Edge* edgeToDelete = dyingEdgePtr.get();
            edges.pop_back();
            dyingEdges.push_back(std::move(dyingEdgePtr));

            ctx.animManager.addAnimation(
                std::make_unique<UI::Animations::EdgeDeleteAnimation>(
                    edgeToDelete, 0.05f,
                    [this, edgeToDelete]() {
                        auto dyingIt = std::find_if(dyingEdges.begin(), dyingEdges.end(),
                            [edgeToDelete](const std::unique_ptr<Edge>& e) { return e.get() == edgeToDelete; });
                        if (dyingIt != dyingEdges.end()) dyingEdges.erase(dyingIt);
                    }
                )
            );
        }
    }

    void Graph::clearEdgesSilently() {
        edges.clear();
        dyingEdges.clear();
    }

    void Graph::resetVisuals(){
        for (auto& node : nodes) {
            node->setFillColor(Config::UI::Colors::NodeFill);
            // Chỉ reset outline nếu node không bị khóa
            if (lockedNodes.find(node.get()) == lockedNodes.end()) {
                node->setOutlineColor(Config::UI::Colors::NodeOutline);
            }
            node->setScale(1.f);
            node->setLabelColor(Config::UI::Colors::NodeText);
        }

        for (auto& edge : edges) {
            if (!edge) continue;
            edge->setColor(Config::UI::Colors::EdgeFill);
            edge->setThickness(Config::UI::EDGE_THICKNESS);
        }
    }

    void Graph::handleEvent(const sf::Event& event, sf::Vector2f mousePos) {
        if (!isDraggable) return;

        // BẮT SỰ KIỆN NHẤN CHUỘT
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                for (int i = drawOrder.size() - 1; i >= 0; --i) {
                    auto node = drawOrder[i]; 
                    if (node->contains(mousePos)) {
                        draggedNode = node;
                        dragOffset = node->getPosition() - mousePos;
                        
                        // Lưu lại vị trí ban đầu và reset cờ Drag
                        initialClickPos = mousePos; 
                        hasDragged = false;         
                        
                        drawOrder.erase(drawOrder.begin() + i);
                        drawOrder.push_back(node);
                        break; 
                    }
                }
            }
        }

        // BẮT SỰ KIỆN NHẢ CHUỘT
        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                if (draggedNode && !hasDragged) {
                    if (lockedNodes.find(draggedNode) != lockedNodes.end()) {
                        // Mở khóa (Xóa màu đỏ ở đây đi)
                        lockedNodes.erase(draggedNode); 
                    } else {
                        // Khóa lại (Xóa màu đỏ ở đây đi)
                        lockedNodes.insert(draggedNode); 
                        velocities[draggedNode] = {0.f, 0.f};
                    }
                }
                draggedNode = nullptr; 
            }
        }
    }

    void Graph::update(sf::Vector2f mouseWorldPos) {
        // ONLY run physics/interactions when allowed AND not currently animating
        // This prevents physics springs from fighting layout animations (which causes 'bunching')
        if (isDraggable && !isAnimating()) {
            // CÁC HẰNG SỐ VẬT LÝ
            float safeDistance = 140.f;  
            float accelRate = 8.0f;      
            float friction = 0.85f;      
            float springLength = 250.f;   
            float springTension = 0.1f;  

            // 1. Tính LỰC ĐẨY
            std::unordered_map<Node*, sf::Vector2f> forces;
            for (auto& node1 : nodes) {
                Node* n1 = node1.get();
                for (auto& node2 : nodes) {
                    if (node1 == node2) continue;
                    Node* n2 = node2.get();

                    sf::Vector2f delta = n1->getPosition() - n2->getPosition();
                    float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

                    if (dist < safeDistance && dist > 0.001f) {
                        float pushFactor = (safeDistance - dist) / safeDistance;
                        forces[n1] += (delta / dist) * pushFactor * accelRate;
                    }
                }
            }

            // 2. Tính LỰC HÚT
            for (const auto& edge : edges) {
                if (!edge) continue;
                Node* u = edge->getSource();
                Node* v = edge->getDest();
                if (!u || !v) continue;

                sf::Vector2f delta = v->getPosition() - u->getPosition();
                float dist = std::sqrt(delta.x * delta.x + delta.y * delta.y);

                if (dist > springLength) {
                    float pullFactor = (dist - springLength) * springTension;
                    sf::Vector2f pullForce = (delta / dist) * pullFactor;
                    forces[u] += pullForce; 
                    forces[v] -= pullForce; 
                }
            }

            // 3. TÌM NODE ĐANG HOVER
            Node* hoveredNode = nullptr;
            if (draggedNode == nullptr) {
                for (int i = drawOrder.size() - 1; i >= 0; --i) {
                    if (drawOrder[i]->contains(mouseWorldPos)) {
                        hoveredNode = drawOrder[i]; 
                        break; 
                    }
                }
            }

            // 4. ÁP DỤNG VẬT LÝ & KIỂM SOÁT GIAO DIỆN
            for (auto& nodePtr : nodes) {
                Node* n = nodePtr.get();
                
                n->setOutlineThickness(Config::UI::NODE_OUTLINE_THICKNESS);
                n->setOutlineColor(Config::UI::Colors::NodeOutline);

                // TRẠNG THÁI KÉO
                if (n == draggedNode) {
                    sf::Vector2f deltaMouse = mouseWorldPos - initialClickPos;
                    if (std::sqrt(deltaMouse.x * deltaMouse.x + deltaMouse.y * deltaMouse.y) > 3.0f) {
                        hasDragged = true;
                    }
                    velocities[n] = {0.f, 0.f}; 
                    n->setPosition(mouseWorldPos + dragOffset);
                    n->onHover();
                } 
                // TRẠNG THÁI KHÓA
                else if (lockedNodes.find(n) != lockedNodes.end()) {
                    velocities[n] = {0.f, 0.f}; 
                    n->setOutlineThickness(Config::UI::NODE_OUTLINE_THICKNESS * 2.5f); 
                }
                // TRẠNG THÁI VẬT LÝ BÌNH THƯỜNG
                else {
                    velocities[n] += forces[n];                       
                    n->setPosition(n->getPosition() + velocities[n]); 
                    velocities[n] *= friction;                        
                    
                    if (std::abs(velocities[n].x) < 0.1f) velocities[n].x = 0.f;
                    if (std::abs(velocities[n].y) < 0.1f) velocities[n].y = 0.f;
                }

                // TRẠNG THÁI HOVER
                if (n == hoveredNode) {
                    n->onHover(); 
                }
            }
        } else{
            Node* hoveredNode = nullptr;
            if (draggedNode == nullptr) {
                for (int i = drawOrder.size() - 1; i >= 0; --i) {
                    if (drawOrder[i]->contains(mouseWorldPos)) {
                        hoveredNode = drawOrder[i]; 
                        break; 
                    }
                }
            }

            for (auto n: drawOrder){
                if (n == hoveredNode){
                    n->onHover();
                } else{
                    n->onIdle();
                }
            }
        }

        // 5. LUÔN CẬP NHẬT CẠNH (EDGES)
        // Dù đang chạy thuật toán (không drag) hay chạy vật lý, cạnh vẫn phải luôn bám sát theo node
        for (auto& edge : edges) {
            edge->update();
        }
    }
    void Graph::draw() {
        for (auto& edge : edges) {
            edge->draw();
        }
        for (auto& edge : dyingEdges) {
            edge->draw();
        }
        for (const auto &node: drawOrder) {
            node->draw();
        }
    }

    const std::vector<std::unique_ptr<Node>>& Graph::getNodes() const { return nodes; }
    const std::vector<std::unique_ptr<Edge>>& Graph::getEdges() const { return edges; }
    std::vector<std::unique_ptr<Edge>>& Graph::getEdges() { return edges; }

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
    
    bool Graph::isNodeLocked(Node* node) const {
        return lockedNodes.find(node) != lockedNodes.end();
    }

    void Graph::setNodeLocked(Node* node, bool locked) {
        if (!node) return;
        if (locked) {
            lockedNodes.insert(node);
            velocities[node] = {0.f, 0.f};
            node->setOutlineThickness(Config::UI::NODE_OUTLINE_THICKNESS * 2.5f); // Làm viền dày lên
        } else {
            lockedNodes.erase(node);
            node->setOutlineThickness(Config::UI::NODE_OUTLINE_THICKNESS); // Trả về bình thường
        }
    }

} // namespace UI::DSA