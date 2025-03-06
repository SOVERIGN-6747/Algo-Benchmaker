#include "../include/Maze.hpp"
#include "../include/pathfinders/AStar.hpp"
#include "../include/pathfinders/Dijkstra.hpp"
#include "../include/pathfinders/BFS.hpp"
#include "../include/pathfinders/DFS.hpp"
#include "../include/pathfinders/GreedyBestFirst.hpp"
#include "../include/pathfinders/JPS.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class TestRunner {
public:
    void runAllTests() {
        std::cout << "Running pathfinding algorithm tests...\n";
        
        testSimpleMaze();
        testNoPath();
        testLargeMaze();
        testEmptyMaze();
        
        std::cout << "All tests passed!\n";
    }

private:
    void testSimpleMaze() {
        std::cout << "Testing simple maze... ";
        
        // Create a simple 5x5 maze
        Maze maze;
        maze.resize(5, 5);
        
        // Set walls to create a simple path
        maze.setCellType(1, 1, Maze::CellType::WALL);
        maze.setCellType(1, 2, Maze::CellType::WALL);
        maze.setCellType(1, 3, Maze::CellType::WALL);
        maze.setCellType(3, 1, Maze::CellType::WALL);
        maze.setCellType(3, 2, Maze::CellType::WALL);
        maze.setCellType(3, 3, Maze::CellType::WALL);
        
        // Set start and goal
        maze.setCellType(0, 0, Maze::CellType::START);
        maze.setCellType(4, 4, Maze::CellType::GOAL);

        // Test each algorithm
        testAlgorithm<AStarEuclidean>(maze, true);
        testAlgorithm<Dijkstra>(maze, true);
        testAlgorithm<BFS>(maze, true);
        testAlgorithm<DFS>(maze, true);
        testAlgorithm<GreedyBestFirst>(maze, true);
        testAlgorithm<JPS>(maze, true);

        std::cout << "PASSED\n";
    }

    void testNoPath() {
        std::cout << "Testing maze with no path... ";
        
        // Create a maze with no possible path
        Maze maze;
        maze.resize(5, 5);
        
        // Create a wall that blocks all paths
        for (int y = 0; y < 5; ++y) {
            maze.setCellType(2, y, Maze::CellType::WALL);
        }
        
        // Set start and goal on opposite sides of the wall
        maze.setCellType(0, 2, Maze::CellType::START);
        maze.setCellType(4, 2, Maze::CellType::GOAL);

        // Test each algorithm
        testAlgorithm<AStarEuclidean>(maze, false);
        testAlgorithm<Dijkstra>(maze, false);
        testAlgorithm<BFS>(maze, false);
        testAlgorithm<DFS>(maze, false);
        testAlgorithm<GreedyBestFirst>(maze, false);
        testAlgorithm<JPS>(maze, false);

        std::cout << "PASSED\n";
    }

    void testLargeMaze() {
        std::cout << "Testing large maze... ";
        
        // Create a large maze
        Maze maze;
        maze.resize(100, 100);
        maze.generateRandom(100, 100, 0.3);

        // Test each algorithm
        testAlgorithm<AStarEuclidean>(maze, true);
        testAlgorithm<Dijkstra>(maze, true);
        testAlgorithm<BFS>(maze, true);
        testAlgorithm<DFS>(maze, true);
        testAlgorithm<GreedyBestFirst>(maze, true);
        testAlgorithm<JPS>(maze, true);

        std::cout << "PASSED\n";
    }

    void testEmptyMaze() {
        std::cout << "Testing empty maze... ";
        
        // Create an empty maze
        Maze maze;
        maze.resize(10, 10);
        
        // Set start and goal
        maze.setCellType(0, 0, Maze::CellType::START);
        maze.setCellType(9, 9, Maze::CellType::GOAL);

        // Test each algorithm
        testAlgorithm<AStarEuclidean>(maze, true);
        testAlgorithm<Dijkstra>(maze, true);
        testAlgorithm<BFS>(maze, true);
        testAlgorithm<DFS>(maze, true);
        testAlgorithm<GreedyBestFirst>(maze, true);
        testAlgorithm<JPS>(maze, true);

        std::cout << "PASSED\n";
    }

    template<typename Algorithm>
    void testAlgorithm(const Maze& maze, bool shouldFindPath) {
        Algorithm algorithm;
        auto result = algorithm.findPath(maze);
        
        if (shouldFindPath) {
            assert(!result.path.empty() && "Algorithm should find a path");
            assert(result.path.front() == maze.getStart() && "Path should start at start point");
            assert(result.path.back() == maze.getGoal() && "Path should end at goal point");
            
            // Verify path continuity
            for (size_t i = 1; i < result.path.size(); ++i) {
                auto& current = result.path[i];
                auto& previous = result.path[i-1];
                int dx = std::abs(current.x - previous.x);
                int dy = std::abs(current.y - previous.y);
                assert((dx <= 1 && dy <= 1) && "Path should be continuous");
            }
        } else {
            assert(result.path.empty() && "Algorithm should not find a path");
        }
    }
};

int main() {
    try {
        TestRunner runner;
        runner.runAllTests();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
} 