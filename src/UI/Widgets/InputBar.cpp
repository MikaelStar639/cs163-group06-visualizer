#include "UI/Widgets/InputBar.hpp"


namespace UI::Widgets {

InputBar::InputBar( AppContext& context,
                    sf::Vector2f pos,
                    sf::Vector2f size,
                    const std::string& placeholder,
                    InputType inputType)
    :   ctx(context),
        box(size, Config::UI::BUTTON_CORNER_RADIUS),
        text(ctx.font, "", Config::UI::FONT_SIZE_BUTTON),
        placeholderText(ctx.font, placeholder, Config::UI::FONT_SIZE_BUTTON),
        errorText(ctx.font, "", 18),
        idleColor(Config::UI::Colors::InputBarIdle),
        focusedColor(Config::UI::Colors::InputBarFocuse),
        invalidColor(sf::Color(55, 40, 40)),
        invalidOutlineColor(sf::Color::Red),
        errorColor(sf::Color::Red),
        outlineColor(Config::UI::Colors::InputBarOutline),
        focusedOutlineColor(Config::UI::Colors::InputBarFocusedOutline),
        textColor(Config::UI::Colors::InputBarText),
        placeholderColor(Config::UI::Colors::InputBarPlaceholder),
        type(inputType)
{
    box.setPosition(pos);
    box.setOutlineThickness(Config::UI::BUTTON_OUTLINE);
    box.setFillColor(idleColor);
    box.setOutlineColor(outlineColor);

    text.setFillColor(textColor);
    placeholderText.setFillColor(placeholderColor);
    errorText.setFillColor(errorColor);

    cursor.setSize({2.f, text.getCharacterSize() + 5.f});
    cursor.setFillColor(sf::Color::White);

    setType(inputType);
    updateTextPositions();
}

//validate input data function
bool InputBar::isCharacterAllowed(char32_t unicode) const {
    if (unicode < 32 || unicode > 126) return false;

    char c = static_cast<char>(unicode);

    switch (type) {
        case InputType::AnyText:
            return true;

        case InputType::Integer: {
        if (c == '-') {
            return content.empty(); // only allow just one '-' at the first place
        }

        if (!std::isdigit(static_cast<unsigned char>(c))) {
            return false;
        }

        // NO leading zero
        // bloac "01", "007", "-01",...
        if (content == "0" || content == "-0") {
            return false;
        }

        // new string after input
        std::string candidate = content + c;

        // Not allow only "-"
        if (candidate == "-") {
            return true;
        }

        try {
            std::size_t pos = 0;
            int value = std::stoi(candidate, &pos);

            // parse all the current string
            if (pos != candidate.size()) {
                return false;
            }

            // range is -999 to 999
            return value >= -999 && value <= 999;
        }
        catch (...) {
            return false;
        }
    }
             

        case InputType::IntegerList:
        case InputType::EdgeTriple:
            return std::isdigit(static_cast<unsigned char>(c)) || c == '-' || c == ' ';

        case InputType::Word:
            return std::isalpha(static_cast<unsigned char>(c));
    }

    return false;
}

//validate all the string!, (avoid "--12" )
bool InputBar::validateContent() {
    isValid = true;
    errorMessage.clear();

    if (content.empty()) {
        errorText.setString("");
        return true;
    }

    try {
        switch (type) {
            case InputType::AnyText:
                break;

            case InputType::Word:
                if (content.size() > 20) {
                    isValid = false;
                    errorMessage = "Maximum 20 letters";
                    break;
                }
                for (char c : content) {
                    if (!std::isalpha(static_cast<unsigned char>(c))) {
                        isValid = false;
                        errorMessage = "Only letters are allowed";
                        break;
                    }
                }
                break;

            case InputType::Integer: {
                std::size_t pos = 0;
                int value = std::stoi(content, &pos);
                if (pos != content.size()) {
                    isValid = false;
                    errorMessage = "Invalid integer";
                }

                if (value < -999 || value > 999) {
                    isValid = false;
                    errorMessage = "Value must be from -999 to 999";
                }
                break;
            }

            case InputType::IntegerList: {
                std::stringstream ss(content);
                int x;
                bool hasNumber = false;

                while (ss >> x) {
                    hasNumber = true;
                }

                if (!hasNumber || !ss.eof()) {
                    isValid = false;
                    errorMessage = "Enter integers separated by spaces";
                }
                break;
            }

            case InputType::EdgeTriple: {
                std::stringstream ss(content);
                int u, v, w;
                char extra;

                if (!(ss >> u >> v >> w) || (ss >> extra)) {
                    isValid = false;
                    errorMessage = "Format: u v w";
                }
                break;
            }
        }
    } catch (...) {
        isValid = false;
        switch (type) {
            case InputType::Integer:
                errorMessage = "Invalid integer";
                break;
            case InputType::IntegerList:
                errorMessage = "Enter integers separated by spaces";
                break;
            case InputType::EdgeTriple:
                errorMessage = "Format: u v w";
                break;
            default:
                errorMessage = "Invalid input";
                break;
        }
    }

    errorText.setString(errorMessage);
    updateTextPositions();
    return isValid;
}

void InputBar::updateTextPositions() {
    sf::Vector2f pos    = box.getPosition();
    sf::Vector2f size   = box.getSize();

    float paddingX      = 15.f;
    float centerY       = pos.y + size.y / 2.f;
    fitTextToBox(text);
    fitTextToBox(placeholderText);

    sf::FloatRect textBounds            = text.getLocalBounds();
    text.setOrigin({
        0.f,
        textBounds.position.y + textBounds.size.y / 2.f
    });
    text.setPosition({pos.x + paddingX, centerY});

    sf::FloatRect placeholderBounds     = placeholderText.getLocalBounds();
    placeholderText.setOrigin({
        0.f,
        placeholderBounds.position.y + placeholderBounds.size.y / 2.f
    });
    placeholderText.setPosition({pos.x + paddingX, centerY});

    float cursorX;
    

    if (content.empty()) {
        cursorX = placeholderText.getPosition().x;
    } else {
        sf::FloatRect global = text.getGlobalBounds();
        cursorX = global.position.x + global.size.x + 2.f;
    }

    float cursorY = box.getPosition().y + box.getSize().y / 2.f - cursor.getSize().y / 2.f;

    cursor.setPosition({cursorX, cursorY});
}

void InputBar::handleEvent(const sf::Event& event) {
    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()){
        if (mousePressed->button == sf::Mouse::Button::Left){
            sf::Vector2f mousePos(
                static_cast<float>(mousePressed->position.x),
                static_cast<float>(mousePressed->position.y)
            );

            isFocused = box.getGlobalBounds().contains(mousePos);

            if (isFocused) {
                cursorClock.restart();
                showCursor = true;
            }
        }
    }
    if (!isFocused) return;

    if (const auto* textEntered = event.getIf<sf::Event::TextEntered>()) {
        char32_t unicode = textEntered->unicode;

        if (unicode == 8)  {
            if (!content.empty()) {
                content.pop_back();
                text.setString(content);
                validateContent();
                updateTextPositions();
                cursorClock.restart();
                showCursor = true;
                
            }
        }
        else if (isCharacterAllowed(unicode)) {
            if (content.size() < maxLength) {
                content.push_back(static_cast<char> (unicode));
                text.setString(content);
                validateContent();
                updateTextPositions();
                cursorClock.restart();
                showCursor = true;
            }
        }
        
    }
}

void InputBar::update() {
    if (isFocused) {
        if (cursorClock.getElapsedTime().asSeconds() >= cursorBlinkTime ) {
            showCursor = !showCursor;
            cursorClock.restart();
        }
    } else {
        showCursor = false;
    }

    if (!isValid) {
        box.setFillColor(invalidColor);
        box.setOutlineColor(invalidOutlineColor);
    } else if (isFocused) {
        box.setFillColor(focusedColor);
        box.setOutlineColor(focusedOutlineColor);
    } else {
        box.setFillColor(idleColor);
        box.setOutlineColor(outlineColor);
    }
}

void InputBar::draw(){
    ctx.window.draw(box);

    if (content.empty()) {
        ctx.window.draw(placeholderText);
    } else {
        ctx.window.draw(text);
    }

    if (isFocused && showCursor) {
        ctx.window.draw(cursor);
    }

}


// setter
void InputBar::setPosition(sf::Vector2f pos) {
    box.setPosition(pos);
    updateTextPositions();
}

void InputBar::setSize(sf::Vector2f size) {
    box.setSize(size);
    updateTextPositions();
}

void InputBar::setPlaceholder(const std::string& placeholder) {
    placeholderText.setString(placeholder);
    updateTextPositions();
}

void InputBar::setText(const std::string& value) {
    content = value;
    text.setString(content);
    updateTextPositions();
}

void InputBar::setMaxLength(std::size_t length) {
    maxLength = length;
}

void InputBar::setFocus(bool focus) {
    isFocused = focus;
    if (isFocused) {
        cursorClock.restart();
        showCursor = true;
    }
}

void InputBar::setType(InputType inputType) {
    type = inputType;
    switch (type) {
        case InputType::Integer:
            maxLength = 4;   // -999 đến 999
            break;

        case InputType::Word:
            maxLength = 20;
            break;

        case InputType::AnyText:
            maxLength = 50;
            break;
        case InputType::IntegerList:
        case InputType::EdgeTriple:
            maxLength = 20;
            break;
    }
    validateContent();
}

void InputBar::fitTextToBox(sf::Text& target, float horizontalPadding, float verticalPadding) {
    sf::Vector2f size = box.getSize();

    unsigned int maxCharSize = Config::UI::FONT_SIZE_BUTTON;
    unsigned int minCharSize = 10;

    target.setCharacterSize(maxCharSize);

    while (target.getCharacterSize() > minCharSize) {
        sf::FloatRect bounds = target.getLocalBounds();

        float maxWidth  = size.x - 2.f * horizontalPadding;
        float maxHeight = size.y - 2.f * verticalPadding;

        if (bounds.size.x <= maxWidth && bounds.size.y <= maxHeight) {
            break;
        }

        target.setCharacterSize(target.getCharacterSize() - 1);
    }
}

// getter
const std::string& InputBar::getText() const {
    return content;
}
bool InputBar::valid() const {
    return isValid;
}

const std::string& InputBar::getErrorMessage() const {
    return errorMessage;
}


bool InputBar::empty() const {
    return content.empty();
}

bool InputBar::focused() const {
    return isFocused;
}

void InputBar::clear() {
    content.clear();
    text.setString(content);
    errorMessage.clear();
    errorText.setString("");
    isValid = true;
    updateTextPositions();
}




bool InputBar::isSubmitted(const sf::Event& event) const{
    if (!isFocused) return false;

    if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        return keyPressed->code == sf::Keyboard::Key::Enter;
    }
    return false;
}

}