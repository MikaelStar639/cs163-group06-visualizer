#pragma once
#include "UI/DSA/Node.hpp"
#include "UI/DSA/Edge.hpp"
#include "Core/AppContext.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cstddef>
#include <unordered_map>
#include <unordered_set> 

namespace UI::DSA {

    class Graph {
    private:
        AppContext& ctx;
        
        std::vector<std::unique_ptr<Node>> nodes;
        std::vector<std::unique_ptr<Node>> dyingNodes;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<std::unique_ptr<Edge>> dyingEdges;
        std::vector<Node*> drawOrder;

        std::unordered_map<Node*, sf::Vector2f> velocities;
        std::unordered_set<Node*> lockedNodes;

        Node* draggedNode = nullptr;
        sf::Vector2f dragOffset;

        bool isDirected;
        bool isDraggable; 

        bool hasDragged = false;
        sf::Vector2f initialClickPos;

    public:
        Graph(AppContext& context, bool directed = false);

        void setDraggable(bool draggable);
        bool getDraggable() const;

        void addNode(const std::string& val, sf::Vector2f pos);
        Node* addNodeRaw(const std::string& val, sf::Vector2f pos);
        void insertNodeAt(int index, const std::string& val, sf::Vector2f pos); 
        void removeLastNode(); 
        void removeNodeAt(int index); 
        std::unique_ptr<Node> extractNode(int index);
        void insertNodePtr(int index, std::unique_ptr<Node> node);
        void updateNodeValue(int index, const std::string &newVal);
        
        void addEdge(int srcIndex, int destIndex, const std::string& weight = "", bool animate = true);
        void removeEdge(int srcIndex, int destIndex);
        void removeEdgeAt(int index, bool animate = true);

        void clear();      
        void clearEdges(); 
        void clearEdgesSilently(); 

        void resetVisuals();

        void handleEvent(const sf::Event& event, sf::Vector2f mousePos);
        void update(sf::Vector2f mouseWorldPos);
        void draw();

        const std::vector<std::unique_ptr<Node>>& getNodes() const;
        const std::vector<std::unique_ptr<Edge>>& getEdges() const;
        std::vector<std::unique_ptr<Edge>>& getEdges();

        Node* getNode(int index) const; 
        Edge* getEdge(int srcIndex, int destIndex) const; 
        bool isAnimating() const;
        sf::FloatRect getGraphBounds() const;
        
        bool getIsDirected() const;

        // setter
        void setNodeValueRaw(int index, const std::string& newVal);

        void swapNodePointers(int i, int j);

        void setIsDirected(bool Directed);

        size_t getNodeCount() const {return nodes.size();}
        bool isNodeLocked(Node* node) const;
        void setNodeLocked(Node* node, bool locked);
    };

} // namespace UI::DSA