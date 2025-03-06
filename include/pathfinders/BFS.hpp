#pragma once

#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <bits/stdc++.h>

class BFS : public Pathfinder {
public:
    PathfindingResult findPath(const Maze& maze, bool visualize = false, 
                              VisualizationCallback callback = nullptr) override {
        auto start_time = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.nodesExplored = 0;

        Maze::Point start = maze.getStart();
        Maze::Point goal = maze.getGoal();

        std::queue<Maze::Point> queue;
        std::unordered_map<Maze::Point, Maze::Point> cameFrom;
        std::unordered_set<Maze::Point> visited;

        queue.push(start);
        visited.insert(start);

        while (!queue.empty()) {
            Maze::Point current = queue.front();
            queue.pop();
            result.nodesExplored++;

            // Visualize current step
            if (visualize) {
                std::vector<Maze::Point> currentPath;
                if (!cameFrom.empty()) {
                    currentPath = reconstructPath(cameFrom, start, current);
                }
                visualizeStep(current, currentPath, visualize, callback);
            }

            if (current == goal) {
                result.path = reconstructPath(cameFrom, start, goal);
                result.pathLength = result.path.size();
                break;
            }

            for (const Maze::Point& neighbor : getNeighbors(maze, current)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    cameFrom[neighbor] = current;
                    queue.push(neighbor);
                }
            }
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        result.memoryUsage = getCurrentMemoryUsage();

        return result;
    }

    std::string getName() const override {
        return "Breadth-First Search";
    }
}; 