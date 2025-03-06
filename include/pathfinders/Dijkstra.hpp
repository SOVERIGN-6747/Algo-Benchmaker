#pragma once

#include "Pathfinder.hpp"
#include <queue>
#include <unordered_map>
#include <bits/stdc++.h>

class Dijkstra : public Pathfinder {
public:
    struct Node {
        Maze::Point point;
        double distance;

        Node(const Maze::Point& p, double d)
            : point(p), distance(d) {}

        bool operator>(const Node& other) const {
            return distance > other.distance;
        }
    };

    PathfindingResult findPath(const Maze& maze, bool visualize = false, 
                              VisualizationCallback callback = nullptr) override {
        auto start_time = std::chrono::high_resolution_clock::now();
        PathfindingResult result;
        result.nodesExplored = 0;

        Maze::Point start = maze.getStart();
        Maze::Point goal = maze.getGoal();

        // Priority queue for vertices
        std::priority_queue<Node, std::vector<Node>, std::greater<Node>> pq;
        
        // Maps for tracking
        std::unordered_map<Maze::Point, Maze::Point> cameFrom;
        std::unordered_map<Maze::Point, double> distance;
        
        // Initialize distances
        pq.push(Node(start, 0.0));
        distance[start] = 0.0;

        while (!pq.empty()) {
            Node current = pq.top();
            pq.pop();
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

            // If we've found a longer path, skip
            if (distance.count(current.point) && current.distance > distance[current.point]) {
                continue;
            }

            // Check all neighbors
            for (const Maze::Point& neighbor : getNeighbors(maze, current.point, true)) {
                double newDist = current.distance + 1.0; // Using uniform cost for simplicity

                if (!distance.count(neighbor) || newDist < distance[neighbor]) {
                    distance[neighbor] = newDist;
                    cameFrom[neighbor] = current.point;
                    pq.push(Node(neighbor, newDist));
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
        return "Dijkstra";
    }
}; 