#pragma once

#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <bits/stdc++.h>

class GreedyBestFirst : public Pathfinder {
public:
    struct Node {
        Maze::Point point;
        double heuristic;

        Node(const Maze::Point& p, double h)
            : point(p), heuristic(h) {}

        bool operator>(const Node& other) const {
            return heuristic > other.heuristic;
        }
    };

    PathfindingResult findPath(const Maze& maze, bool visualize = false, 
                              VisualizationCallback callback = nullptr) override {
        auto start_time = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.nodesExplored = 0;

        Maze::Point start = maze.getStart();
        Maze::Point goal = maze.getGoal();

        // Priority queue for open set
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openSet;
        
        // Maps for tracking
        std::unordered_map<Maze::Point, Maze::Point> cameFrom;
        std::unordered_set<Maze::Point> visited;
        
        // Initialize start node
        openSet.push(Node(start, heuristic(start, goal)));
        visited.insert(start);

        while (!openSet.empty()) {
            Node current = openSet.top();
            openSet.pop();
            result.nodesExplored++;

            // Visualize current step
            if (visualize) {
                std::vector<Maze::Point> currentPath;
                if (!cameFrom.empty()) {
                    currentPath = reconstructPath(cameFrom, start, current.point);
                }
                visualizeStep(current.point, currentPath, visualize, callback);
            }

            // Found the goal
            if (current.point == goal) {
                result.path = reconstructPath(cameFrom, start, goal);
                result.pathLength = result.path.size();
                break;
            }

            // Check neighbors
            for (const Maze::Point& neighbor : getNeighbors(maze, current.point, true)) {
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    cameFrom[neighbor] = current.point;
                    openSet.push(Node(neighbor, heuristic(neighbor, goal)));
                }
            }
        }

        // Calculate execution time
        auto end_time = std::chrono::high_resolution_clock::now();
        result.executionTime = std::chrono::duration<double, std::milli>(end_time - start_time).count();
        
        // Get memory usage
        result.memoryUsage = getCurrentMemoryUsage();

        return result;
    }

    std::string getName() const override {
        return "Greedy Best-First Search";
    }

private:
    double heuristic(const Maze::Point& a, const Maze::Point& b) const {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}; 