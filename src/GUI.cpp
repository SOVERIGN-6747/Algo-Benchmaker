#include "GUI.hpp"
#include "pathfinders/AStar.hpp"
#include "pathfinders/Dijkstra.hpp"
#include "pathfinders/BFS.hpp"
#include "pathfinders/DFS.hpp"
#include "pathfinders/GreedyBestFirst.hpp"
#include "pathfinders/JPS.hpp"
#include "Benchmark.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

GUI::GUI(int windowWidth, int windowHeight)
    : cellSize_(20.f)
    , currentAlgorithm_(0)
    , isRunning_(false)
    , isDragging_(false)
    , currentDrawMode_(Maze::CellType::WALL)
{
    initWindow(windowWidth, windowHeight);
    initUI();
    initAlgorithms();
    
    // Initialize maze with a default size
    maze_.resize(40, 30);
    maze_.generateRandom(40, 30, 0.3f);
}

void GUI::initWindow(int width, int height) {
    window_.create(sf::VideoMode(width, height), "Pathfinding Visualizer");
    window_.setFramerateLimit(60);

    // Create views for maze and UI
    mazeView_.reset(sf::FloatRect(0, 0, width, height - UI_PANEL_HEIGHT));
    uiView_.reset(sf::FloatRect(0, height - UI_PANEL_HEIGHT, width, UI_PANEL_HEIGHT));
    
    mazeView_.setViewport(sf::FloatRect(0, 0, 1, (height - UI_PANEL_HEIGHT) / height));
    uiView_.setViewport(sf::FloatRect(0, (height - UI_PANEL_HEIGHT) / height, 1, UI_PANEL_HEIGHT / height));
}

void GUI::initUI() {
    if (!font_.loadFromFile("resources/arial.ttf")) {
        throw std::runtime_error("Could not load font");
    }

    // Initialize algorithm name text
    algorithmName_.setFont(font_);
    algorithmName_.setCharacterSize(20);
    algorithmName_.setFillColor(sf::Color::White);
    algorithmName_.setPosition(10, window_.getSize().y - UI_PANEL_HEIGHT + 10);

    // Initialize metrics text
    metrics_.setFont(font_);
    metrics_.setCharacterSize(16);
    metrics_.setFillColor(sf::Color::White);
    metrics_.setPosition(10, window_.getSize().y - UI_PANEL_HEIGHT + 40);

    // Initialize buttons and dropdown
    initializeButtons();
    initializeDropdown();
}

void GUI::initializeButtons() {
    float buttonX = window_.getSize().x - BUTTON_WIDTH - BUTTON_PADDING;
    float buttonY = window_.getSize().y - UI_PANEL_HEIGHT + (UI_PANEL_HEIGHT - BUTTON_HEIGHT) / 2;

    // Run Algorithm button
    runButton_.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    runButton_.setPosition(buttonX, buttonY);
    runButton_.setFillColor(sf::Color(100, 100, 200));

    runButtonText_.setFont(font_);
    runButtonText_.setString("Run Algorithm");
    runButtonText_.setCharacterSize(16);
    runButtonText_.setFillColor(sf::Color::White);
    centerTextInButton(runButtonText_, runButton_);

    // Benchmark button
    benchmarkButton_.setSize(sf::Vector2f(BUTTON_WIDTH, BUTTON_HEIGHT));
    benchmarkButton_.setPosition(buttonX - BUTTON_WIDTH - BUTTON_PADDING, buttonY);
    benchmarkButton_.setFillColor(sf::Color(100, 200, 100));

    benchmarkButtonText_.setFont(font_);
    benchmarkButtonText_.setString("Benchmark All");
    benchmarkButtonText_.setCharacterSize(16);
    benchmarkButtonText_.setFillColor(sf::Color::White);
    centerTextInButton(benchmarkButtonText_, benchmarkButton_);
}

void GUI::initializeDropdown() {
    float dropdownX = 10;
    float dropdownY = 10;

    // Initialize dropdown background
    algorithmDropdown_.background.setSize(sf::Vector2f(DROPDOWN_WIDTH, DROPDOWN_ITEM_HEIGHT));
    algorithmDropdown_.background.setPosition(dropdownX, dropdownY);
    algorithmDropdown_.background.setFillColor(sf::Color(70, 70, 70));
    algorithmDropdown_.background.setOutlineThickness(1);
    algorithmDropdown_.background.setOutlineColor(sf::Color::White);

    // Initialize selected text
    algorithmDropdown_.selectedText.setFont(font_);
    algorithmDropdown_.selectedText.setCharacterSize(16);
    algorithmDropdown_.selectedText.setFillColor(sf::Color::White);
    algorithmDropdown_.selectedText.setPosition(
        dropdownX + 5,
        dropdownY + (DROPDOWN_ITEM_HEIGHT - 16) / 2
    );

    // Initialize dropdown state
    algorithmDropdown_.isOpen = false;
    algorithmDropdown_.itemHeight = DROPDOWN_ITEM_HEIGHT;

    // Create options
    float optionY = dropdownY + DROPDOWN_ITEM_HEIGHT;
    for (const auto& algorithm : algorithms_) {
        sf::Text option;
        option.setFont(font_);
        option.setString(algorithm->getName());
        option.setCharacterSize(16);
        option.setFillColor(sf::Color::White);
        option.setPosition(dropdownX + 5, optionY + (DROPDOWN_ITEM_HEIGHT - 16) / 2);
        
        algorithmDropdown_.options.push_back(option);
        optionY += DROPDOWN_ITEM_HEIGHT;
    }

    // Set initial selected algorithm
    if (!algorithms_.empty()) {
        algorithmDropdown_.selectedText.setString(algorithms_[currentAlgorithm_]->getName());
    }
}

void GUI::centerTextInButton(sf::Text& text, const sf::RectangleShape& button) {
    sf::FloatRect textBounds = text.getLocalBounds();
    text.setPosition(
        button.getPosition().x + (button.getSize().x - textBounds.width) / 2,
        button.getPosition().y + (button.getSize().y - textBounds.height) / 2
    );
}

void GUI::initAlgorithms() {
    // Add all pathfinding algorithms
    algorithms_.push_back(std::make_unique<AStarEuclidean>());
    algorithms_.push_back(std::make_unique<Dijkstra>());
    algorithms_.push_back(std::make_unique<BFS>());
    algorithms_.push_back(std::make_unique<DFS>());
    algorithms_.push_back(std::make_unique<GreedyBestFirst>());
    algorithms_.push_back(std::make_unique<JPS>());
}

void GUI::run() {
    while (window_.isOpen()) {
        handleEvents();
        draw();
    }
}

void GUI::handleEvents() {
    sf::Event event;
    while (window_.pollEvent(event)) {
        switch (event.type) {
            case sf::Event::Closed:
                window_.close();
                break;

            case sf::Event::MouseButtonPressed:
                handleMouseClick(event);
                break;

            case sf::Event::MouseButtonReleased:
                isDragging_ = false;
                break;

            case sf::Event::MouseMoved:
                if (isDragging_) {
                    handleMouseDrag(sf::Mouse::getPosition(window_));
                }
                break;

            case sf::Event::KeyPressed:
                handleKeyPress(event);
                break;

            default:
                break;
        }
    }
}

void GUI::handleMouseClick(const sf::Event& event) {
    sf::Vector2i mousePos(event.mouseButton.x, event.mouseButton.y);
    
    // Check if clicked on the dropdown
    if (mousePos.y < UI_PANEL_HEIGHT) {
        handleDropdownClick(mousePos);
        return;
    }

    // Check if clicked on the run button
    if (runButton_.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        runAlgorithm();
        return;
    }

    // Check if clicked on the benchmark button
    if (benchmarkButton_.getGlobalBounds().contains(mousePos.x, mousePos.y)) {
        runBenchmark();
        return;
    }

    // Check if clicked in the maze area
    if (mousePos.y < window_.getSize().y - UI_PANEL_HEIGHT) {
        isDragging_ = true;
        handleMouseDrag(mousePos);
    }
}

void GUI::handleMouseDrag(const sf::Vector2i& mousePos) {
    sf::Vector2i gridPos = windowToGrid(mousePos);
    if (maze_.isValidCell(gridPos.x, gridPos.y)) {
        maze_.setCellType(gridPos.x, gridPos.y, currentDrawMode_);
    }
}

void GUI::handleKeyPress(const sf::Event& event) {
    switch (event.key.code) {
        case sf::Keyboard::Space:
            runAlgorithm();
            break;

        case sf::Keyboard::Tab:
            nextAlgorithm();
            break;

        case sf::Keyboard::R:
            generateRandomMaze();
            break;

        case sf::Keyboard::C:
            clearMaze();
            break;

        case sf::Keyboard::W:
            currentDrawMode_ = Maze::CellType::WALL;
            break;

        case sf::Keyboard::S:
            currentDrawMode_ = Maze::CellType::START;
            break;

        case sf::Keyboard::G:
            currentDrawMode_ = Maze::CellType::GOAL;
            break;

        case sf::Keyboard::P:
            currentDrawMode_ = Maze::CellType::PATH;
            break;

        case sf::Keyboard::L:
            if (event.key.control) {
                loadMaze();
            }
            break;

        case sf::Keyboard::O:
            if (event.key.control) {
                saveMaze();
            }
            break;

        default:
            break;
    }
}

void GUI::draw() {
    window_.clear(sf::Color(50, 50, 50));

    // Draw maze
    window_.setView(mazeView_);
    drawMaze();

    // Draw UI
    window_.setView(uiView_);
    drawUI();

    window_.display();
}

void GUI::drawMaze() {
    maze_.draw(window_, cellSize_);
}

void GUI::drawUI() {
    // Draw algorithm dropdown
    drawDropdown();

    // Draw metrics
    window_.draw(metrics_);

    // Draw buttons
    window_.draw(runButton_);
    window_.draw(runButtonText_);
    window_.draw(benchmarkButton_);
    window_.draw(benchmarkButtonText_);
}

void GUI::drawDropdown() {
    // Draw dropdown background
    window_.draw(algorithmDropdown_.background);
    window_.draw(algorithmDropdown_.selectedText);

    // Draw options if dropdown is open
    if (algorithmDropdown_.isOpen) {
        float optionY = algorithmDropdown_.background.getPosition().y + DROPDOWN_ITEM_HEIGHT;
        
        for (const auto& option : algorithmDropdown_.options) {
            sf::RectangleShape optionBackground(sf::Vector2f(DROPDOWN_WIDTH, DROPDOWN_ITEM_HEIGHT));
            optionBackground.setPosition(algorithmDropdown_.background.getPosition().x, optionY);
            optionBackground.setFillColor(sf::Color(50, 50, 50));
            optionBackground.setOutlineThickness(1);
            optionBackground.setOutlineColor(sf::Color::White);

            window_.draw(optionBackground);
            window_.draw(option);

            optionY += DROPDOWN_ITEM_HEIGHT;
        }
    }
}

void GUI::updateMetrics() {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << "Time: " << lastResult_.executionTime << "ms | "
        << "Nodes: " << lastResult_.nodesExplored << " | "
        << "Path: " << lastResult_.pathLength << " | "
        << "Memory: " << lastResult_.memoryUsage / 1024 << "KB";
    metrics_.setString(oss.str());
}

sf::Vector2i GUI::windowToGrid(const sf::Vector2i& windowPos) const {
    return sf::Vector2i(
        static_cast<int>(windowPos.x / cellSize_),
        static_cast<int>(windowPos.y / cellSize_)
    );
}

void GUI::runAlgorithm() {
    if (!algorithms_.empty()) {
        lastResult_ = algorithms_[currentAlgorithm_]->findPath(maze_);
        updateMetrics();
    }
}

void GUI::nextAlgorithm() {
    if (!algorithms_.empty()) {
        currentAlgorithm_ = (currentAlgorithm_ + 1) % algorithms_.size();
    }
}

void GUI::generateRandomMaze() {
    maze_.generateRandom(maze_.getWidth(), maze_.getHeight(), 0.3f);
}

void GUI::clearMaze() {
    maze_.clear();
}

void GUI::loadMaze() {
    // Create a file dialog (in a real implementation, you would use a proper file dialog library)
    std::string filename;
    std::cout << "Enter maze file path (ASCII .txt or PNG): ";
    std::getline(std::cin, filename);

    if (filename.empty()) return;

    bool success = false;
    if (filename.ends_with(".txt")) {
        success = maze_.loadFromASCII(filename);
    } else if (filename.ends_with(".png")) {
        success = maze_.loadFromPNG(filename);
    }

    if (success) {
        metrics_.setString("Maze loaded successfully from " + filename);
    } else {
        metrics_.setString("Failed to load maze from " + filename);
    }
}

void GUI::saveMaze() {
    // Create a file dialog (in a real implementation, you would use a proper file dialog library)
    std::string filename;
    std::cout << "Enter PNG file path to save maze: ";
    std::getline(std::cin, filename);

    if (filename.empty()) return;

    if (!filename.ends_with(".png")) {
        filename += ".png";
    }

    if (maze_.saveToPNG(filename)) {
        metrics_.setString("Maze saved successfully to " + filename);
    } else {
        metrics_.setString("Failed to save maze to " + filename);
    }
}

void GUI::runBenchmark() {
    // Create benchmark instance
    Benchmark benchmark(algorithms_);
    
    // Run benchmarks
    auto results = benchmark.runBenchmarks(maze_, 10, true);
    
    // Export results
    benchmark.exportToCSV(results, "benchmark_results.csv");
    benchmark.exportToMarkdown(results, "benchmark_results.md");
    
    // Update metrics with benchmark summary
    std::ostringstream oss;
    oss << "Benchmark completed. Results saved to benchmark_results.csv and benchmark_results.md";
    metrics_.setString(oss.str());
}

void GUI::handleDropdownClick(const sf::Vector2i& mousePos) {
    sf::FloatRect dropdownBounds = algorithmDropdown_.background.getGlobalBounds();
    
    // Check if clicked on the dropdown header
    if (dropdownBounds.contains(mousePos.x, mousePos.y)) {
        algorithmDropdown_.isOpen = !algorithmDropdown_.isOpen;
        return;
    }

    // Check if clicked on an option
    if (algorithmDropdown_.isOpen) {
        float optionY = dropdownBounds.top + DROPDOWN_ITEM_HEIGHT;
        for (size_t i = 0; i < algorithmDropdown_.options.size(); ++i) {
            sf::FloatRect optionBounds(
                dropdownBounds.left,
                optionY,
                DROPDOWN_WIDTH,
                DROPDOWN_ITEM_HEIGHT
            );

            if (optionBounds.contains(mousePos.x, mousePos.y)) {
                currentAlgorithm_ = i;
                algorithmDropdown_.selectedText.setString(algorithms_[i]->getName());
                algorithmDropdown_.isOpen = false;
                break;
            }

            optionY += DROPDOWN_ITEM_HEIGHT;
        }
    }
} 