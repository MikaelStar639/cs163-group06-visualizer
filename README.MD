# DSA Visualizer

A Data Structures and Algorithms visualizer that built using **C++** and the **SFML** graphics library. This project allows users to interactively explore and visualize complex algorithmic operations through a smooth, frame-independent animation system.

## Key Features

- **Data structures:**
  - **Linked List:** Visualize insertions, erasures, searches and updates.
  - **Heap:** Binary heap operations, including push, pop and building heap.
  - **Trie:** Visualize prefix tree insertions, erasures and searches.
  - **Minimum Spanning Tree (MST):** Interactive graph-based algorithm visualization with 2 algorithms: **Kruskal** and **Prim**.
- **Advanced Step-by-Step Navigation:**
  - Control algorithm execution with Play, Pause, Next, Previous, and Fast Forward controls.
  - Synchronized state snapshots to ensure visual consistency during navigation.
- **Modern Interactive UI:**
  - Custom-built widgets including `InputBar`, `Dropdown` menus, `Sliders` for speed control, and `Buttons`.
  - Integrated **PseudoCode Viewer** to track code execution alongside the visual representation.

## Project Structure

The project follows a modular directory structure for better maintainability:

```text
├── assets/                  # Fonts (SpaceMono.ttf)
├── external/                # External libraries (SFML 3.0.2)
├── include/                 # Header files (.hpp)
│   ├── Core/                # DSA logic, App Context, and Constants
│   ├── Controllers/         # Logic controllers bridging UI and Core
│   ├── States/              # Screen-specific states (Menu, Trie, MST, etc.)
│   └── UI/                  # UI Widgets, Node/Edge visuals, and Animation system
├── src/                     # Source files (.cpp)
└── CMakeLists.txt           # Automated build configuration
```

## Requirements

- g++ (MinGW-w64) 14.2.0 or higher
- CMake 3.15 or higher

## Build Instructions

Open your terminal in th e root project directory and execute the following commands:

```bash
# 1. Create build directory
mkdir build
cd build

# 2. Configure the project
cmake ..

# 3. Compile the executable
cmake --build .

# 4. Launch the application
.\main
```
