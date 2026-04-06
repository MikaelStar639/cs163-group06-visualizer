#include "UI/DSA/Graph.hpp"
#include "UI/Animations/Node/NodeScaleAnimation.hpp"
#include "UI/Animations/Edge/EdgeScaleAnimation.hpp"
#include <algorithm>
#include <iostream>

namespace UI::DSA {

    Graph::Graph(AppContext& context, bool directed)
        : ctx(context), isDirected(directed), isDraggable(true) {}

    void Graph::setDraggable(bool draggable) {
        isDraggable = draggable;
        if (!isDraggable) draggedNodeIndex = -1; // Thả Node ra ngay nếu đang giữ
    }

    bool Graph::getDraggable() const { return isDraggable; }

    void Graph::addNode(const std::string& val, sf::Vector2f pos) {
        nodes.push_back(std::make_unique<Node>(ctx, val, pos));
        
        int newNodeIndex = nodes.size() - 1;
        drawOrder.push_back(newNodeIndex);

        // Animation sinh Node
        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeInsertAnimation>(nodes.back().get(), 0.2f)
        );
    }

    void Graph::removeLastNode() {
        if (nodes.empty()) return;

        Node* nodeToDeletePtr = nodes.back().get();
        int nodeIndexToDelete = nodes.size() - 1;

        // Animation thu nhỏ trước khi xóa data
        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::NodeDeleteAnimation>(
                nodeToDeletePtr, 0.2f, 
                [this, nodeToDeletePtr, nodeIndexToDelete]() { 
                    if (!nodes.empty()) {
                        // 1. Xóa tất cả Edges liên quan đến Node này
                        edges.erase(
                            std::remove_if(edges.begin(), edges.end(),
                                [nodeToDeletePtr](const std::unique_ptr<Edge>& edge) {
                                    return edge->connectsTo(nodeToDeletePtr);
                                }),
                            edges.end()
                        );

                        // 2. Dọn dẹp drawOrder an toàn
                        auto it = std::find(drawOrder.begin(), drawOrder.end(), nodeIndexToDelete);
                        if (it != drawOrder.end()) {
                            drawOrder.erase(it);
                        }

                        // 3. Xóa Data
                        nodes.pop_back();       
                    }
                }
            )
        );
    }

    void Graph::addEdge(int srcIndex, int destIndex, const std::string& weight) {
        if (srcIndex < 0 || srcIndex >= nodes.size() || 
            destIndex < 0 || destIndex >= nodes.size()) return;

        edges.push_back(std::make_unique<Edge>(
            ctx, nodes[srcIndex].get(), nodes[destIndex].get(), isDirected, weight
        ));

        // Animation mọc Cạnh
        ctx.animManager.addAnimation(
            std::make_unique<UI::Animations::EdgeInsertAnimation>(edges.back().get(), 0.3f)
        );
    }

    void Graph::clear() {
        edges.clear();
        nodes.clear();
        drawOrder.clear();
        draggedNodeIndex = -1;
    }

    void Graph::clearEdges() {
        // Cực kỳ hữu dụng cho Heap. Mỗi lần Sift-Up/Sift-Down, 
        // ta chỉ cần clearEdges() và nối lại mảng, thay vì tìm từng cạnh để update.
        edges.clear();
    }

    void Graph::handleEvent(const sf::Event& event, sf::Vector2f mousePos) {
        // Nếu bị khóa, KHÔNG cho phép kéo thả
        if (!isDraggable) return;

        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonPressed>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                // Duyệt ngược drawOrder để ưu tiên chọn Node nằm trên cùng
                for (int i = drawOrder.size() - 1; i >= 0; --i) {
                    int nodeIdx = drawOrder[i]; 
                    if (nodes[nodeIdx]->contains(mousePos)) {
                        draggedNodeIndex = nodeIdx;
                        dragOffset = nodes[nodeIdx]->getPosition() - mousePos;
                        
                        // Đưa Node được bấm lên lớp vẽ trên cùng
                        drawOrder.erase(drawOrder.begin() + i);
                        drawOrder.push_back(nodeIdx);
                        break; 
                    }
                }
            }
        }

        if (const auto* mouseEvent = event.getIf<sf::Event::MouseButtonReleased>()) {
            if (mouseEvent->button == sf::Mouse::Button::Left) {
                draggedNodeIndex = -1;
            }
        }
    }

    void Graph::update() {
        sf::Vector2i mousePosi = sf::Mouse::getPosition(ctx.window);
        sf::Vector2f mousePos = static_cast<sf::Vector2f>(mousePosi);

        // 1. Cập nhật Kéo/Thả & Hover
        if (draggedNodeIndex != -1 && isDraggable) {
            nodes[draggedNodeIndex]->setPosition(mousePos + dragOffset);
            nodes[draggedNodeIndex]->onHover(); // Giữ sáng khi đang kéo
        } else {
            int hoveredNodeIdx = -1;
            for (int i = drawOrder.size() - 1; i >= 0; --i) {
                int realIdx = drawOrder[i];
                if (nodes[realIdx]->contains(mousePos)) {
                    hoveredNodeIdx = realIdx; 
                    break; 
                }
            }
            // Update trạng thái hover
            for (int i = 0; i < nodes.size(); ++i) {
                if (i == hoveredNodeIdx) nodes[i]->onHover(); 
                else nodes[i]->onIdle(); 
            }
        }

        // 2. Cập nhật vị trí của Cạnh bám theo Node
        for (auto& edge : edges) {
            edge->update();
        }
    }

    void Graph::draw() {
        // Vẽ cạnh trước (nằm dưới)
        for (auto& edge : edges) {
            edge->draw();
        }
        // Vẽ Node sau (nằm trên)
        for (int idx : drawOrder) {
            nodes[idx]->draw();
        }
    }

} // namespace UI::DSA