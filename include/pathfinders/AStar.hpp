#pragma once

#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <bits/stdc++.h>

class AStarEuclidean : public Pathfinder {
public:
    struct Node {
        Maze::Point point;
        double f_score;  // f = g + h
        double g_score;  // cost from start to current node

        Node(const Maze::Point& p, double f, double g)
            : point(p), f_score(f), g_score(g) {}

        bool operator>(const Node& other) const {
            return f_score > other.f_score;
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
        std::unordered_map<Maze::Point, double> gScore;
        
        // Initialize start node
        openSet.push(Node(start, heuristic(start, goal), 0.0));
        gScore[start] = 0.0;

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
                // Calculate tentative g score
                double tentative_g = current.g_score + distance(current.point, neighbor);

                // If this path is better than previous ones
                if (!gScore.count(neighbor) || tentative_g < gScore[neighbor]) {
                    cameFrom[neighbor] = current.point;
                    gScore[neighbor] = tentative_g;
                    double f = tentative_g + heuristic(neighbor, goal);
                    openSet.push(Node(neighbor, f, tentative_g));
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
        return "A* (Euclidean)";
    }

private:
    double heuristic(const Maze::Point& a, const Maze::Point& b) const {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    double distance(const Maze::Point& a, const Maze::Point& b) const {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}; 