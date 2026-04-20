#pragma once
#include "UI/DSA/Node.hpp"
#include "UI/DSA/Edge.hpp"
#include "Core/AppContext.hpp"
#include <vector>
#include <memory>
#include <string>
#include <cstddef>

namespace UI::DSA {

    class Graph {
    private:
        AppContext& ctx;
        
        std::vector<std::unique_ptr<Node>> nodes;
        std::vector<std::unique_ptr<Node>> dyingNodes;
        std::vector<std::unique_ptr<Edge>> edges;
        std::vector<Node*> drawOrder;

        Node* draggedNode = nullptr;
        sf::Vector2f dragOffset;

        bool isDirected;
        bool isDraggable; 

    public:
        Graph(AppContext& context, bool directed = false);

        void setDraggable(bool draggable);
        bool getDraggable() const;

        void addNode(const std::string& val, sf::Vector2f pos);
        void insertNodeAt(int index, const std::string& val, sf::Vector2f pos); 
        void removeLastNode(); 
        void removeNodeAt(int index); 
        void updateNodeValue(int index, const std::string &newVal);
        
        void addEdge(int srcIndex, int destIndex, const std::string& weight = "");
        void removeEdge(int srcIndex, int destIndex);

        void clear();      
        void clearEdges(); 

        void resetVisuals();

        void handleEvent(const sf::Event& event, sf::Vector2f mousePos);
        void update(sf::Vector2f mouseWorldPos);
        void draw();

        const std::vector<std::unique_ptr<Node>>& getNodes() const;
        const std::vector<std::unique_ptr<Edge>>& getEdges() const;

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
    };

} // namespace UI::DSA