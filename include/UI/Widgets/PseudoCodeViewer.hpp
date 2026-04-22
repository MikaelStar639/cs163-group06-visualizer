#pragma once
#include <SFML/Graphics.hpp>
#include "UI/Shapes/RoundedRectangleShape.hpp"
#include "Core/DSA/PseudoCodeData.hpp"
#include <vector>
#include <string>

namespace UI::Widgets {

    class PseudoCodeViewer {
    private:
        sf::Font& font;

        RoundedRectangleShape background;
        sf::RectangleShape highlightBar;
        std::vector<sf::Text> lines;
        std::vector<std::string> codeStrings;

        int activeLine = -1;
        bool visible = false;

        sf::Vector2f position;
        float lineHeight = 36.f;
        float paddingX = 24.f;
        float paddingY = 20.f;
        unsigned int fontSize = 26;

        // Anchor system for auto-repositioning
        bool anchoredBottomRight = false;
        float anchorWindowW = 0.f;
        float anchorWindowH = 0.f;
        float anchorMargin  = 20.f;

        // Colors
        sf::Color bgColor        = sf::Color(20, 20, 20, 220);
        sf::Color highlightColor = sf::Color(255, 165, 0, 80);
        sf::Color textNormal     = sf::Color(200, 200, 200);
        sf::Color textHighlight  = sf::Color(255, 255, 255);

        void rebuildTexts();
        void recalcSize();

    public:
        PseudoCodeViewer(sf::Font& font);

        void setCode(const std::vector<std::string>& code);
        void setCode(const Core::DSA::PseudoCodeDef& def);
        void highlightLine(int index);
        void clearHighlight();
        void hide();
        bool isVisible() const { return visible; }
        int getActiveLine() const { return activeLine; }

        void setPosition(float x, float y);
        void setPositionBottomRight(float windowWidth, float windowHeight, float margin = 20.f);

        void draw(sf::RenderWindow& window);
    };

} // namespace UI::Widgets
