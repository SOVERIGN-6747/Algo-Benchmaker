#pragma once

#include <SFML/Graphics.hpp>
#include "Maze.hpp"
#include "pathfinders/Pathfinder.hpp"
#include <memory>
#include <vector>
#include <string>
#include <bits/stdc++.h>


class GUI {
public:
    GUI(int windowWidth = 1200, int windowHeight = 800);
    void run();

private:
    // Window and view management
    sf::RenderWindow window_;
    sf::View mazeView_;
    sf::View uiView_;
    float cellSize_;
    
    // Maze and pathfinding
    Maze maze_;
    std::vector<std::unique_ptr<Pathfinder>> algorithms_;
    size_t currentAlgorithm_;
    Pathfinder::PathfindingResult lastResult_;
    
    // UI elements
    sf::Font font_;
    sf::Text algorithmName_;
    sf::Text metrics_;
    
    // Buttons
    sf::RectangleShape runButton_;
    sf::Text runButtonText_;
    sf::RectangleShape benchmarkButton_;
    sf::Text benchmarkButtonText_;
    
    // Dropdown menu
    struct DropdownMenu {
        sf::RectangleShape background;
        sf::Text selectedText;
        std::vector<sf::Text> options;
        bool isOpen;
        float itemHeight;
    } algorithmDropdown_;
    
    // UI states
    bool isRunning_;
    bool isDragging_;
    Maze::CellType currentDrawMode_;
    bool isVisualizing_;
    std::vector<Maze::Point> currentPath_;
    Maze::Point currentNode_;
    
    // Initialize methods
    void initWindow(int width, int height);
    void initUI();
    void initializeButtons();
    void initializeDropdown();
    void initAlgorithms();
    void centerTextInButton(sf::Text& text, const sf::RectangleShape& button);
    
    // Event handling
    void handleEvents();
    void handleMouseClick(const sf::Event& event);
    void handleMouseDrag(const sf::Vector2i& mousePos);
    void handleKeyPress(const sf::Event& event);
    void handleDropdownClick(const sf::Vector2i& mousePos);
    
    // Drawing methods
    void draw();
    void drawMaze();
    void drawUI();
    void drawDropdown();
    void updateMetrics();
    
    // Helper methods
    sf::Vector2i windowToGrid(const sf::Vector2i& windowPos) const;
    void runAlgorithm();
    void runBenchmark();
    void nextAlgorithm();
    void generateRandomMaze();
    void clearMaze();
    void loadMaze();
    void saveMaze();
    
    // Constants
    const float UI_PANEL_HEIGHT = 100.f;
    const float BUTTON_WIDTH = 150.f;
    const float BUTTON_HEIGHT = 40.f;
    const float BUTTON_PADDING = 10.f;
    const float DROPDOWN_WIDTH = 200.f;
    const float DROPDOWN_ITEM_HEIGHT = 30.f;
}; 