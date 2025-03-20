cmake_minimum_required(VERSION 3.10)
project(PathfindingBenchmark)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find SFML
find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# Add source files
set(SOURCES
    src/main.cpp
    src/Maze.cpp
    src/GUI.cpp
    src/Benchmark.cpp
)

# Add header files
set(HEADERS
    include/Maze.hpp
    include/GUI.hpp
    include/Benchmark.hpp
    include/pathfinders/Pathfinder.hpp
    include/pathfinders/AStar.hpp
    include/pathfinders/Dijkstra.hpp
    include/pathfinders/BFS.hpp
    include/pathfinders/DFS.hpp
    include/pathfinders/GreedyBestFirst.hpp
    include/pathfinders/JPS.hpp
)

# Create main executable
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE include)

# Link SFML
target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-window sfml-system)

# Create test executable
add_executable(${PROJECT_NAME}_test tests/TestRunner.cpp ${HEADERS} src/Maze.cpp)
target_include_directories(${PROJECT_NAME}_test PRIVATE include)
target_link_libraries(${PROJECT_NAME}_test sfml-graphics sfml-window sfml-system) 
