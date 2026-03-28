#include "UI/Widgets/Dropdown.hpp"

namespace UI {
namespace Widgets {

Dropdown::Dropdown(AppContext& context, const std::string& label, sf::Vector2f position, sf::Vector2f size, const std::vector<std::string>& options) 
    : ctx(context), isDropped(false), selectedIndex(-1), mainText(context.font)
{
    // Setup Main Box
    mainBox.setPosition(position);
    mainBox.setSize(size);
    mainBox.setFillColor(sf::Color(50, 50, 50));
    mainBox.setOutlineColor(sf::Color::White);
    mainBox.setOutlineThickness(1.f);

    // Setup Arrow Box
    float arrowAreaWidth = size.y; 
    arrowBox.setSize(sf::Vector2f(arrowAreaWidth, size.y));
    arrowBox.setPosition(sf::Vector2f(position.x + size.x - arrowAreaWidth, position.y));
    arrowBox.setFillColor(sf::Color(70, 70, 70)); // Dark Gray
    arrowBox.setOutlineColor(sf::Color::White);
    arrowBox.setOutlineThickness(1.f);

    // Setup Triangle
    arrowTriangle.setPointCount(3);
    arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
    arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
    arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));
    arrowTriangle.setFillColor(sf::Color::White);
    arrowTriangle.setPosition(sf::Vector2f(
        arrowBox.getPosition().x + (arrowAreaWidth - 16.f) / 2.f,
        arrowBox.getPosition().y + (size.y - 10.f) / 2.f
    ));

    // Setup Main Text
    mainText.setCharacterSize(static_cast<unsigned int>(size.y * 0.45f));
    mainText.setFillColor(sf::Color::White);
    mainText.setString(label);
    
    // Canh giữa chữ trong khu vực không tính ô mũi tên
    sf::FloatRect mainBounds = mainText.getLocalBounds();
    float mainTextX = position.x + (size.x - size.y - mainBounds.size.x) / 2.f;
    mainText.setPosition(sf::Vector2f(mainTextX, position.y + size.y * 0.25f));

    // Setup Options
    for (size_t i = 0; i < options.size(); ++i) {
        RoundedRectangleShape box;
        box.setPosition(sf::Vector2f(position.x, position.y + size.y * (i + 1)));
        box.setSize(size);
        box.setFillColor(sf::Color(50, 50, 50));
        box.setOutlineColor(sf::Color::White);
        box.setOutlineThickness(1.f);
        itemBoxes.push_back(box);

        sf::Text text(ctx.font);
        text.setCharacterSize(mainText.getCharacterSize());
        text.setFillColor(sf::Color::White);
        text.setString(options[i]);
        
        // Canh giữa chữ trong ô Option
        sf::FloatRect textBounds = text.getLocalBounds();
        float textX = box.getPosition().x + (size.x - textBounds.size.x) / 2.f;
        text.setPosition(sf::Vector2f(textX, box.getPosition().y + size.y * 0.25f));
        itemTexts.push_back(text);
    }
}

void Dropdown::update(sf::Vector2i mousePos) {
    sf::Vector2f mappedMouse = ctx.window.mapPixelToCoords(mousePos);

    if (isDropped) {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 10.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 10.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 0.f));

        // Hiệu ứng Click: Khi danh sách đang bị thả xuống, nút chính sẽ tối đi (Pressed state)
        mainBox.setFillColor(sf::Color(30, 30, 30));
        arrowBox.setFillColor(sf::Color(50, 50, 50));
    } else if (mainBox.getGlobalBounds().contains(mappedMouse)) {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));

        mainBox.setFillColor(sf::Color(80, 80, 80));
        arrowBox.setFillColor(sf::Color(100, 100, 100));
    } else {
        arrowTriangle.setPoint(0, sf::Vector2f(0.f, 0.f));
        arrowTriangle.setPoint(1, sf::Vector2f(16.f, 0.f));
        arrowTriangle.setPoint(2, sf::Vector2f(8.f, 10.f));

        mainBox.setFillColor(sf::Color(50, 50, 50));
        arrowBox.setFillColor(sf::Color(70, 70, 70));
    }

    if (isDropped) {
        for (size_t i = 0; i < itemBoxes.size(); ++i) {
            if (itemBoxes[i].getGlobalBounds().contains(mappedMouse)) {
                itemBoxes[i].setFillColor(sf::Color(100, 100, 100)); // Highlight lighter gray
            } else {
                itemBoxes[i].setFillColor(sf::Color(50, 50, 50));
            }
        }
    }
}

void Dropdown::handleEvent(const sf::Event::MouseButtonPressed& event) {
    if (event.button == sf::Mouse::Button::Left) {
        sf::Vector2f mousePos = ctx.window.mapPixelToCoords(sf::Mouse::getPosition(ctx.window));

        bool clickedInsideMainBox = mainBox.getGlobalBounds().contains(mousePos);

        // 1. Toggle when clicking main box
        if (clickedInsideMainBox) {
            isDropped = !isDropped;
            return;
        }

        // 2. Click outside or select item
        if (isDropped) {
            bool clickedAnyItem = false;

            for (size_t i = 0; i < itemBoxes.size(); ++i) {
                if (itemBoxes[i].getGlobalBounds().contains(mousePos)) {
                    selectedIndex = static_cast<int>(i);
                    isDropped = false;
                    clickedAnyItem = true;
                    break;
                }
            }

            if (!clickedAnyItem) {
                isDropped = false; // Click outside
            }
        }
    }
}

void Dropdown::draw() const {
    ctx.window.draw(mainBox);
    ctx.window.draw(arrowBox);
    ctx.window.draw(arrowTriangle);
    ctx.window.draw(mainText);

    if (isDropped) {
        for (const auto& box : itemBoxes) {
            ctx.window.draw(box);
        }
        for (const auto& text : itemTexts) {
            ctx.window.draw(text);
        }
    }
}

bool Dropdown::getIsDropped() const {
    return isDropped;
}

int Dropdown::getSelectedIndex() const {
    return selectedIndex;
}

std::string Dropdown::getSelectedText() const {
    if (selectedIndex >= 0 && selectedIndex < itemTexts.size()) {
        return itemTexts[selectedIndex].getString();
    }
    return "";
}

} // namespace Widgets
} // namespace UI
