#pragma once

#include "pathfinders/Pathfinder.hpp"
#include "Maze.hpp"
#include <vector>
#include <memory>
#include <bits/stdc++.h>
#include <fstream>
#include <iomanip>

class Benchmark {
public:
    struct BenchmarkResult {
        std::string algorithmName;
        double averageTime;
        size_t averageNodesExplored;
        size_t averagePathLength;
        size_t averageMemoryUsage;
    };

    Benchmark(const std::vector<std::unique_ptr<Pathfinder>>& algorithms)
        : algorithms_(algorithms) {}

    std::vector<BenchmarkResult> runBenchmarks(const Maze& maze, int iterations, bool visualize = false) {
        std::vector<BenchmarkResult> results;
        
        for (const auto& algorithm : algorithms_) {
            BenchmarkResult result;
            result.algorithmName = algorithm->getName();
            result.averageTime = 0;
            result.averageNodesExplored = 0;
            result.averagePathLength = 0;
            result.averageMemoryUsage = 0;

            for (int i = 0; i < iterations; ++i) {
                auto pathResult = algorithm->findPath(maze);
                result.averageTime += pathResult.executionTime;
                result.averageNodesExplored += pathResult.nodesExplored;
                result.averagePathLength += pathResult.pathLength;
                result.averageMemoryUsage += pathResult.memoryUsage;
            }

            result.averageTime /= iterations;
            result.averageNodesExplored /= iterations;
            result.averagePathLength /= iterations;
            result.averageMemoryUsage /= iterations;

            results.push_back(result);
        }

        return results;
    }

    void exportToCSV(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;

        // Write header
        file << "Algorithm,Time (ms),Nodes Explored,Path Length,Memory Usage (bytes)\n";

        // Write data
        for (const auto& result : results) {
            file << result.algorithmName << ","
                 << std::fixed << std::setprecision(2) << result.averageTime << ","
                 << result.averageNodesExplored << ","
                 << result.averagePathLength << ","
                 << result.averageMemoryUsage << "\n";
        }
    }

    void exportToMarkdown(const std::vector<BenchmarkResult>& results, const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) return;

        // Write header
        file << "# Pathfinding Algorithm Benchmark Results\n\n";
        file << "| Algorithm | Time (ms) | Nodes Explored | Path Length | Memory Usage (KB) |\n";
        file << "|-----------|-----------|----------------|-------------|------------------|\n";

        // Write data
        for (const auto& result : results) {
            file << "| " << result.algorithmName << " | "
                 << std::fixed << std::setprecision(2) << result.averageTime << " | "
                 << result.averageNodesExplored << " | "
                 << result.averagePathLength << " | "
                 << (result.averageMemoryUsage / 1024) << " |\n";
        }

        // Add timestamp
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        file << "\n\nBenchmark run at: " << std::ctime(&time);
    }

private:
    const std::vector<std::unique_ptr<Pathfinder>>& algorithms_;
}; 