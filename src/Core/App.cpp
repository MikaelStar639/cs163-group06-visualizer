#include "Core/App.hpp"
#include "UI/Widgets/Dropdown.hpp"
#include <bits/stdc++.h>

App::App(): window(sf::VideoMode({1600, 900}), "Visualizer", sf::Style::Default ^ sf::Style::Resize),
            font("assets/fonts/SpaceMono.ttf"),
            context{window, font, ScreenState::None} 
{
    window.setFramerateLimit(60);
    currentScreen = std::make_unique<MenuScreen>(context);
};

// void App::run(){
//     while (window.isOpen()){
//         while (auto event = window.pollEvent()){
//             if (event->is<sf::Event::Closed>()){
//                 window.close();
//                 return;
//             }
            
//             currentScreen->handleEvent(*event);
//         }

//         currentScreen->update();

//         window.clear(Config::Colors::Background); 
//         currentScreen->draw();
//         window.display();

//         if (context.nextState != ScreenState::None){
//             changeScreen(context.nextState);
//             context.nextState = ScreenState::None;
//         }
//     }
// }

void App::run(){
    // 1. Khởi tạo Dropdown ngay trước vòng lặp chính
    Dropdown testDropdown(context, "Select Option", {50.f, 50.f});
    
    // 2. Setup các lựa chọn bên trong
    testDropdown.setOptions({"Option 1", "Option 2", "Option 3"});

    while (window.isOpen()){
        while (auto event = window.pollEvent()){
            if (event->is<sf::Event::Closed>()){
                window.close();
                return;
            }
            
            // Xử lý sự kiện cho Screen hiện tại
            currentScreen->handleEvent(*event);
            
            // 3. Xử lý sự kiện click cho Dropdown
            if (testDropdown.isClicked(*event)) {
                // In ra console tab vừa được chọn để test thử
                std::cout << "Ban vua chon: " << testDropdown.getSelectedText() << "\n";
            }
        }

        // Cập nhật logic màn hình hiện tại
        currentScreen->update();

        // 4. Cập nhật logic của Dropdown (hover, hiệu ứng...)
        testDropdown.update(sf::Mouse::getPosition(window));

        window.clear(Config::Colors::Background); 
        
        // Vẽ màn hình hiện tại trước
        currentScreen->draw();
        
        // 5. Vẽ Dropdown đè lên trên cùng để dễ thấy
        testDropdown.draw();
        
        window.display();

        if (context.nextState != ScreenState::None){
            changeScreen(context.nextState);
            context.nextState = ScreenState::None;
        }
    }
}


void App::changeScreen(ScreenState nextState) {
    if (nextState == ScreenState::None) return;

    switch (nextState) {
        case ScreenState::MainMenu:
            currentScreen = std::make_unique<MenuScreen>(context);
            break;
        // case ScreenState::LinkedList:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::Heap:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::Trie:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;
        // case ScreenState::MST:
        //     currentScreen = std::make_unique<MenuScreen>(context);
        //     break;

        case ScreenState::Exit:
            window.close();
            break;

        default:
            break;
    }
}