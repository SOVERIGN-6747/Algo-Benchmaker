#pragma once

#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <cmath>
#include <bits/stdc++.h>

class JPS : public Pathfinder {
public:
    struct Node {
        Maze::Point point;
        double f_score;
        double g_score;

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

            // Get successors using jump point search
            auto successors = findSuccessors(maze, current.point, goal);
            for (const auto& successor : successors) {
                double tentative_g = current.g_score + distance(current.point, successor);

                if (!gScore.count(successor) || tentative_g < gScore[successor]) {
                    cameFrom[successor] = current.point;
                    gScore[successor] = tentative_g;
                    double f = tentative_g + heuristic(successor, goal);
                    openSet.push(Node(successor, f, tentative_g));
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
        return "Jump Point Search";
    }

private:
    std::vector<Maze::Point> findSuccessors(const Maze& maze, const Maze::Point& node, const Maze::Point& goal) {
        std::vector<Maze::Point> successors;
        
        // Check in all 8 directions
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                if (dx == 0 && dy == 0) continue;

                Maze::Point jumpPoint = jump(maze, node, dx, dy, goal);
                if (jumpPoint.x != -1) {  // Valid jump point found
                    successors.push_back(jumpPoint);
                }
            }
        }
        
        return successors;
    }

    Maze::Point jump(const Maze& maze, const Maze::Point& node, int dx, int dy, const Maze::Point& goal) {
        Maze::Point next(node.x + dx, node.y + dy);

        // Check if next point is valid
        if (!maze.isValidCell(next.x, next.y) || !maze.isWalkable(next.x, next.y)) {
            return Maze::Point(-1, -1);
        }

        // Found goal
        if (next == goal) {
            return next;
        }

        // Check for forced neighbors
        if (hasForcedNeighbor(maze, next, dx, dy)) {
            return next;
        }

        // Diagonal movement
        if (dx != 0 && dy != 0) {
            // Check horizontal and vertical
            if (jump(maze, next, dx, 0, goal).x != -1 || 
                jump(maze, next, 0, dy, goal).x != -1) {
                return next;
            }
        }

        // Continue jumping
        return jump(maze, next, dx, dy, goal);
    }

    bool hasForcedNeighbor(const Maze& maze, const Maze::Point& node, int dx, int dy) {
        // Check for forced neighbors based on direction
        if (dx != 0 && dy != 0) {  // Diagonal movement
            // Check for blocked cells that create forced neighbors
            bool blocked_hor = !maze.isWalkable(node.x - dx, node.y);
            bool blocked_ver = !maze.isWalkable(node.x, node.y - dy);
            return blocked_hor || blocked_ver;
        } else if (dx != 0) {  // Horizontal movement
            return (!maze.isWalkable(node.x, node.y + 1) && maze.isWalkable(node.x + dx, node.y + 1)) ||
                   (!maze.isWalkable(node.x, node.y - 1) && maze.isWalkable(node.x + dx, node.y - 1));
        } else if (dy != 0) {  // Vertical movement
            return (!maze.isWalkable(node.x + 1, node.y) && maze.isWalkable(node.x + 1, node.y + dy)) ||
                   (!maze.isWalkable(node.x - 1, node.y) && maze.isWalkable(node.x - 1, node.y + dy));
        }
        return false;
    }

    double heuristic(const Maze::Point& a, const Maze::Point& b) const {
        double dx = std::abs(a.x - b.x);
        double dy = std::abs(a.y - b.y);
        return std::sqrt(2) * std::min(dx, dy) + std::abs(dx - dy);
    }

    double distance(const Maze::Point& a, const Maze::Point& b) const {
        double dx = a.x - b.x;
        double dy = a.y - b.y;
        return std::sqrt(dx * dx + dy * dy);
    }
}; 