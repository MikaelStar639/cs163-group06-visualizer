#include "States/DSAScreenBase.hpp"
#include <algorithm>

DSAScreenBase::DSAScreenBase(AppContext& context, bool directed) 
    : ctx(context), myGraph(context, directed) 
{
    graphView = ctx.window.getDefaultView();
}


void DSAScreenBase::handleEvent(const sf::Event& event) {
    sf::Vector2i pixelPos = sf::Mouse::getPosition(ctx.window);
    sf::Vector2f worldPos = ctx.window.mapPixelToCoords(pixelPos, graphView);
    myGraph.handleEvent(event, worldPos);

    if (const auto* mousePressed = event.getIf<sf::Event::MouseButtonPressed>()) {
        if (mousePressed->button == sf::Mouse::Button::Right) {
            isPanning = true;
            lastPanMousePos = mousePressed->position; 
        }
    }
    else if (const auto* mouseReleased = event.getIf<sf::Event::MouseButtonReleased>()) {
        if (mouseReleased->button == sf::Mouse::Button::Right) {
            isPanning = false;
        }
    }
    else if (const auto* mouseMoved = event.getIf<sf::Event::MouseMoved>()) {
        if (isPanning) {
            sf::Vector2i currentMousePos = mouseMoved->position;
            
            sf::Vector2f delta = ctx.window.mapPixelToCoords(lastPanMousePos, graphView) - 
                                    ctx.window.mapPixelToCoords(currentMousePos, graphView);
            
            graphView.move(delta);

            sf::FloatRect bounds = myGraph.getGraphBounds();
            float padding = 1000.f; 

            sf::Vector2f center = graphView.getCenter();
            
            center.x = std::clamp(center.x, 
                                    bounds.position.x - padding, 
                                    bounds.position.x + bounds.size.x + padding);
                                    
            center.y = std::clamp(center.y, 
                                    bounds.position.y - padding, 
                                    bounds.position.y + bounds.size.y + padding);
            
            graphView.setCenter(center);

            lastPanMousePos = currentMousePos;
        }
    }
    else if (const auto* wheelScrolled = event.getIf<sf::Event::MouseWheelScrolled>()) {
        if (wheelScrolled->wheel == sf::Mouse::Wheel::Vertical) {
            float zoomAmount = 1.1f;
            if (wheelScrolled->delta > 0) {
                graphView.zoom(1.0f / zoomAmount); 
            } else if (wheelScrolled->delta < 0) {
                graphView.zoom(zoomAmount);        
            }
        }
    }
    else if (const auto* keyPressed = event.getIf<sf::Event::KeyPressed>()) {
        if (keyPressed->code == sf::Keyboard::Key::Space) {
            graphView = ctx.window.getDefaultView();
        }
    }
}

void DSAScreenBase::update() {
    // 1. get the pixel coords of the screen
    sf::Vector2i pixelPos = sf::Mouse::getPosition(ctx.window);
    
    // 2. use the camera to convert to riel coords
    sf::Vector2f worldPos = ctx.window.mapPixelToCoords(pixelPos, graphView);
    
    // 3. Graph manage the hover with the riel coords
    myGraph.update(worldPos); 
}

void DSAScreenBase::draw(){
    ctx.window.setView(graphView);
    myGraph.draw();
    
    ctx.window.setView(ctx.window.getDefaultView());
}

