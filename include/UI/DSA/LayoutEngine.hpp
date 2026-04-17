#pragma once
#include "UI/DSA/Graph.hpp"
#include "UI/Animations/Core/ParallelAnimation.hpp"
#include <memory>

namespace UI::DSA::LayoutEngine {

    // Linked list:
    // Returns a group of animation contains all of the Node moves.
    // startX, startY: Coordinate of the first node
    // spacing: space between nodes
    std::unique_ptr<Animations::ParallelAnimation> asLinkedList(
        const Graph& graph, 
        float startX, 
        float startY, 
        float spacing, 
        float duration = 0.5f
    );

    // Heap:
    std::unique_ptr<Animations::ParallelAnimation> asHeap(
        const Graph& graph,
        float startX,
        float startY,
        float spacing,
        float duration = 0.5f
    );

    // Trie:
    // ...

    // MST:
    // ...
} // namespace UI::DSA::LayoutEngine