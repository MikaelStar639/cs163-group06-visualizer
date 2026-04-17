#pragma once

#include <SFML/Graphics.hpp>
#include <Core/AppContext.hpp>
#include <Core/DSA/Heap.hpp>
#include <States/Screen.hpp>
#include <States/DSAScreenBase.hpp>
#include <UI/Components/HeapMenu.hpp>
#include <UI/Widgets/PseudoCodeViewer.hpp>
#include <UI/DSA/Graph.hpp>
#include <vector>
#include <string>
#include "Controllers/HeapController.hpp"

class HeapScreen : public DSAScreenBase {
private:
    UI::Widgets::HeapMenu uiMenu;
    UI::Widgets::PseudoCodeViewer codeViewer;
    Core::DSA::Heap model;
    Controllers::HeapController controller;

    bool isRawData; // Added to track the "Create" vs "Heapify" state

    void handleMenuAction(int subBtnIndex);

public:
    explicit HeapScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update() override;
    void draw() override;
};