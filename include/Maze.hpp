#pragma once

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>

class Maze {
public:
    struct Point {
        int x, y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
        bool operator==(const Point& other) const {
            return x == other.x && y == other.y;
        }
    };

    enum class CellType {
        PATH,
        WALL,
        START,
        GOAL,
        VISITED,
        PATH_FOUND
    };

    Maze();
    ~Maze() = default;

    // File loading/saving
    bool loadFromASCII(const std::string& filename);
    bool loadFromPNG(const std::string& filename);
    bool saveToPNG(const std::string& filename) const;
    
    // Grid manipulation
    void resize(int width, int height);
    void setCellType(int x, int y, CellType type);
    CellType getCellType(int x, int y) const;
    void clear();
    void generateRandom(int width, int height, float wallDensity);

    // Getters
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    Point getStart() const { return start_; }
    Point getGoal() const { return goal_; }
    bool isValidCell(int x, int y) const;
    bool isWalkable(int x, int y) const;

    // SFML rendering
    void draw(sf::RenderWindow& window, float cellSize) const;

private:
    std::vector<std::vector<CellType>> grid_;
    int width_;
    int height_;
    Point start_;
    Point goal_;

    // Helper functions
    void validatePoint(const Point& p) const;
    void updateStartGoal();
};

// Hash function for Point to use in unordered containers
namespace std {
    template<>
    struct hash<Maze::Point> {
        size_t operator()(const Maze::Point& p) const {
            return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);
        }
    };
} 