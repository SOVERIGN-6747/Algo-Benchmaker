#include "Maze.hpp"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <random>

Maze::Maze() : width_(0), height_(0) {}

bool Maze::loadFromASCII(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) return false;

    std::vector<std::string> lines;
    std::string line;
    
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    if (lines.empty()) return false;

    height_ = lines.size();
    width_ = lines[0].length();
    grid_.resize(height_, std::vector<CellType>(width_, CellType::PATH));

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            char c = lines[y][x];
            switch (c) {
                case '0': grid_[y][x] = CellType::PATH; break;
                case '1': grid_[y][x] = CellType::WALL; break;
                case 'S': 
                    grid_[y][x] = CellType::START;
                    start_ = Point(x, y);
                    break;
                case 'G': 
                    grid_[y][x] = CellType::GOAL;
                    goal_ = Point(x, y);
                    break;
                default: grid_[y][x] = CellType::PATH;
            }
        }
    }

    return true;
}

bool Maze::loadFromPNG(const std::string& filename) {
    sf::Image image;
    if (!image.loadFromFile(filename)) return false;

    width_ = image.getSize().x;
    height_ = image.getSize().y;
    grid_.resize(height_, std::vector<CellType>(width_, CellType::PATH));

    bool foundStart = false;
    bool foundGoal = false;

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            sf::Color pixel = image.getPixel(x, y);
            
            // Convert to grayscale for better wall detection
            float grayscale = (pixel.r * 0.299f + pixel.g * 0.587f + pixel.b * 0.114f) / 255.f;

            // Check specific colors first
            if (pixel.g > 200 && pixel.r < 100 && pixel.b < 100) {  // Green for start
                grid_[y][x] = CellType::START;
                start_ = Point(x, y);
                foundStart = true;
            }
            else if (pixel.r > 200 && pixel.g < 100 && pixel.b < 100) {  // Red for goal
                grid_[y][x] = CellType::GOAL;
                goal_ = Point(x, y);
                foundGoal = true;
            }
            else if (grayscale < 0.5f) {  // Dark pixels are walls
                grid_[y][x] = CellType::WALL;
            }
            else {  // Light pixels are paths
                grid_[y][x] = CellType::PATH;
            }
        }
    }

    // If start/goal not found, set defaults
    if (!foundStart) {
        start_ = Point(0, 0);
        grid_[0][0] = CellType::START;
    }
    if (!foundGoal) {
        goal_ = Point(width_ - 1, height_ - 1);
        grid_[height_ - 1][width_ - 1] = CellType::GOAL;
    }

    return true;
}

bool Maze::saveToPNG(const std::string& filename) const {
    sf::Image image;
    image.create(width_, height_, sf::Color::White);

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            switch (grid_[y][x]) {
                case CellType::WALL:
                    image.setPixel(x, y, sf::Color::Black);
                    break;
                case CellType::START:
                    image.setPixel(x, y, sf::Color::Green);
                    break;
                case CellType::GOAL:
                    image.setPixel(x, y, sf::Color::Red);
                    break;
                case CellType::VISITED:
                    image.setPixel(x, y, sf::Color(255, 255, 0));  // Yellow
                    break;
                case CellType::PATH_FOUND:
                    image.setPixel(x, y, sf::Color::Blue);
                    break;
                default:  // PATH
                    image.setPixel(x, y, sf::Color::White);
                    break;
            }
        }
    }

    return image.saveToFile(filename);
}

void Maze::resize(int width, int height) {
    width_ = width;
    height_ = height;
    grid_.resize(height_, std::vector<CellType>(width_, CellType::PATH));
}

void Maze::setCellType(int x, int y, CellType type) {
    if (!isValidCell(x, y)) return;
    grid_[y][x] = type;
    updateStartGoal();
}

Maze::CellType Maze::getCellType(int x, int y) const {
    if (!isValidCell(x, y)) throw std::out_of_range("Cell coordinates out of bounds");
    return grid_[y][x];
}

void Maze::clear() {
    for (auto& row : grid_) {
        std::fill(row.begin(), row.end(), CellType::PATH);
    }
    start_ = Point(0, 0);
    goal_ = Point(width_ - 1, height_ - 1);
}

void Maze::generateRandom(int width, int height, float wallDensity) {
    resize(width, height);
    clear();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(0.0, 1.0);

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (dis(gen) < wallDensity) {
                grid_[y][x] = CellType::WALL;
            }
        }
    }

    // Ensure start and goal are paths
    grid_[start_.y][start_.x] = CellType::START;
    grid_[goal_.y][goal_.x] = CellType::GOAL;
}

bool Maze::isValidCell(int x, int y) const {
    return x >= 0 && x < width_ && y >= 0 && y < height_;
}

bool Maze::isWalkable(int x, int y) const {
    if (!isValidCell(x, y)) return false;
    return grid_[y][x] != CellType::WALL;
}

void Maze::draw(sf::RenderWindow& window, float cellSize) const {
    sf::RectangleShape cell(sf::Vector2f(cellSize - 1, cellSize - 1));

    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            cell.setPosition(x * cellSize, y * cellSize);

            switch (grid_[y][x]) {
                case CellType::WALL:
                    cell.setFillColor(sf::Color::Black);
                    break;
                case CellType::PATH:
                    cell.setFillColor(sf::Color::White);
                    break;
                case CellType::START:
                    cell.setFillColor(sf::Color::Green);
                    break;
                case CellType::GOAL:
                    cell.setFillColor(sf::Color::Red);
                    break;
                case CellType::VISITED:
                    cell.setFillColor(sf::Color(255, 255, 0, 128)); // Semi-transparent yellow
                    break;
                case CellType::PATH_FOUND:
                    cell.setFillColor(sf::Color::Blue);
                    break;
            }

            window.draw(cell);
        }
    }
}

void Maze::validatePoint(const Point& p) const {
    if (!isValidCell(p.x, p.y)) {
        throw std::out_of_range("Point coordinates out of bounds");
    }
}

void Maze::updateStartGoal() {
    for (int y = 0; y < height_; ++y) {
        for (int x = 0; x < width_; ++x) {
            if (grid_[y][x] == CellType::START) {
                start_ = Point(x, y);
            } else if (grid_[y][x] == CellType::GOAL) {
                goal_ = Point(x, y);
            }
        }
    }
} 