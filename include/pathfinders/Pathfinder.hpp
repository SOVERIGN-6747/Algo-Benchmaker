#pragma once

#include "../Maze.hpp"
#include <vector>
#include <chrono>
#include <string>
#include <memory>
#include <bits/stdc++.h>
#include <functional>
#include <thread>

class Pathfinder {
public:
    // Add callback type for visualization
    using VisualizationCallback = std::function<void(const Maze::Point&, const std::vector<Maze::Point>&)>;
    
    struct PathfindingResult {
        std::vector<Maze::Point> path;
        double executionTime;  // in milliseconds
        size_t nodesExplored;
        size_t pathLength;
        size_t memoryUsage;    // in bytes
    };

    virtual ~Pathfinder() = default;

    // Main solving method that all algorithms must implement
    virtual PathfindingResult findPath(const Maze& maze, bool visualize = false, VisualizationCallback callback = nullptr) = 0;

    // Get algorithm name for display and benchmarking
    virtual std::string getName() const = 0;

    // Optional method to visualize the search process
    virtual void visualizeStep(const Maze::Point& current, const std::vector<Maze::Point>& path, 
                              bool visualize, VisualizationCallback callback) {
        if (visualize && callback) {
            callback(current, path);
            std::this_thread::sleep_for(std::chrono::milliseconds(50)); // Delay for visualization
        }
    }

protected:
    // Helper method to reconstruct path from came_from map
    template<typename CameFromMap>
    std::vector<Maze::Point> reconstructPath(
        const CameFromMap& cameFrom,
        const Maze::Point& start,
        const Maze::Point& current) const {
        
        std::vector<Maze::Point> path;
        Maze::Point current_node = current;
        
        while (current_node != start) {
            path.push_back(current_node);
            auto it = cameFrom.find(current_node);
            if (it == cameFrom.end()) break;
            current_node = it->second;
        }
        path.push_back(start);
        
        std::reverse(path.begin(), path.end());
        return path;
    }

    // Common neighbor directions (4-directional)
    const std::vector<std::pair<int, int>> directions4 = {
        {0, 1},  // down
        {1, 0},  // right
        {0, -1}, // up
        {-1, 0}  // left
    };

    // 8-directional movement (including diagonals)
    const std::vector<std::pair<int, int>> directions8 = {
        {0, 1},   // down
        {1, 0},   // right
        {0, -1},  // up
        {-1, 0},  // left
        {1, 1},   // down-right
        {-1, 1},  // down-left
        {1, -1},  // up-right
        {-1, -1}  // up-left
    };

    // Helper method to get valid neighbors
    std::vector<Maze::Point> getNeighbors(
        const Maze& maze,
        const Maze::Point& current,
        bool allowDiagonal = false) const {
        
        std::vector<Maze::Point> neighbors;
        const auto& dirs = allowDiagonal ? directions8 : directions4;

        for (const auto& [dx, dy] : dirs) {
            Maze::Point next(current.x + dx, current.y + dy);
            if (maze.isValidCell(next.x, next.y) && maze.isWalkable(next.x, next.y)) {
                neighbors.push_back(next);
            }
        }

        return neighbors;
    }

    // Helper method to measure memory usage
    size_t getCurrentMemoryUsage() const {
        // This is a simplified version. In a real implementation,
        // you would use platform-specific methods to get actual memory usage
        return 0;
    }
}; 