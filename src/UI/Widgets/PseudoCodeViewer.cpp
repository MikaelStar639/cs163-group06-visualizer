#include "UI/Widgets/PseudoCodeViewer.hpp"
#include "Core/Constants.hpp"

namespace UI::Widgets {

PseudoCodeViewer::PseudoCodeViewer(sf::Font& font)
    : font(font),
      background({300.f, 100.f}, Config::UI::Radius::Lg),
      highlightBar({300.f, lineHeight})
{
    background.setFillColor(bgColor);
    background.setOutlineThickness(1.f);
    background.setOutlineColor(sf::Color(80, 80, 80, 180));

    highlightBar.setFillColor(highlightColor);
}

void PseudoCodeViewer::setCode(const std::vector<std::string>& code) {
    codeStrings = code;
    activeLine = -1;
    visible = true;
    rebuildTexts();
    recalcSize();
}

void PseudoCodeViewer::highlightLine(int index) {
    if (index < 0 || index >= static_cast<int>(lines.size())) return;

    // Reset previous highlight
    if (activeLine >= 0 && activeLine < static_cast<int>(lines.size())) {
        lines[activeLine].setFillColor(textNormal);
    }

    activeLine = index;
    lines[activeLine].setFillColor(textHighlight);

    // Move highlight bar to the correct Y position
    float barY = position.y + paddingY + activeLine * lineHeight;
    highlightBar.setPosition({position.x, barY});
}

void PseudoCodeViewer::clearHighlight() {
    if (activeLine >= 0 && activeLine < static_cast<int>(lines.size())) {
        lines[activeLine].setFillColor(textNormal);
    }
    activeLine = -1;
}

void PseudoCodeViewer::hide() {
    visible = false;
    clearHighlight();
}

void PseudoCodeViewer::setPosition(float x, float y) {
    position = {x, y};
    background.setPosition(position);
    recalcSize(); // Reposition texts
}

void PseudoCodeViewer::setPositionBottomRight(float windowWidth, float windowHeight, float margin) {
    anchoredBottomRight = true;
    anchorWindowW = windowWidth;
    anchorWindowH = windowHeight;
    anchorMargin = margin;

    sf::Vector2f bgSize = background.getSize();
    float x = windowWidth - bgSize.x - margin;
    float y = windowHeight - bgSize.y - margin;
    setPosition(x, y);
}

void PseudoCodeViewer::rebuildTexts() {
    lines.clear();
    for (size_t i = 0; i < codeStrings.size(); ++i) {
        sf::Text text(font, codeStrings[i], fontSize);
        text.setFillColor(textNormal);
        lines.push_back(std::move(text));
    }
}

void PseudoCodeViewer::recalcSize() {
    if (codeStrings.empty()) return;

    // Find widest line
    float maxWidth = 0.f;
    for (auto& line : lines) {
        float w = line.getLocalBounds().size.x;
        if (w > maxWidth) maxWidth = w;
    }

    float totalWidth = maxWidth + paddingX * 2.f;
    float totalHeight = lineHeight * lines.size() + paddingY * 2.f;

    background.setSize({totalWidth, totalHeight});
    highlightBar.setSize({totalWidth, lineHeight});

    // Auto-reposition if anchored to bottom-right
    if (anchoredBottomRight) {
        position.x = anchorWindowW - totalWidth - anchorMargin;
        position.y = anchorWindowH - totalHeight - anchorMargin;
    }

    // Reposition background
    background.setPosition(position);

    // Reposition each text line
    for (size_t i = 0; i < lines.size(); ++i) {
        float textX = position.x + paddingX;
        float textY = position.y + paddingY + i * lineHeight;
        lines[i].setPosition({textX, textY});
    }

    // Reposition highlight bar if active
    if (activeLine >= 0) {
        float barY = position.y + paddingY + activeLine * lineHeight;
        highlightBar.setPosition({position.x, barY});
    }
}

void PseudoCodeViewer::draw(sf::RenderWindow& window) {
    if (!visible || lines.empty()) return;

    window.draw(background);

    if (activeLine >= 0) {
        window.draw(highlightBar);
    }

    for (auto& line : lines) {
        window.draw(line);
    }
}

} // namespace UI::Widgets
