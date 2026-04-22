#pragma once
#include "States/DSAScreenBase.hpp"
#include "UI/Components/TrieMenu.hpp"      
#include "Core/DSA/Trie.hpp"               
#include "Controllers/TrieController.hpp"  
#include "UI/Widgets/PseudoCodeViewer.hpp" 

class TrieScreen : public DSAScreenBase {
private:
    UI::Widgets::TrieMenu uiMenu;
    UI::Widgets::PseudoCodeViewer codeViewer; 
    Core::DSA::Trie model;
    Controllers::TrieController controller;

    void handleMenuAction();

public:
    explicit TrieScreen(AppContext& context);

    void handleEvent(const sf::Event& event) override;
    void update(float dt) override;
    void draw() override;
};