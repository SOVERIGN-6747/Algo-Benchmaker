# Pathfinding Algorithm Visualizer and Benchmark

A C++ SFML application for visualizing and benchmarking various pathfinding algorithms. This project was created for the CICR Robo-Soccer team to optimize robot navigation logic.

## Features

### Algorithms Implemented
- A* (Euclidean heuristic)
- Dijkstra's Algorithm
- Breadth-First Search (BFS)
- Depth-First Search (DFS)
- Greedy Best-First Search
- Jump Point Search (JPS)

### Visualization Features
- Interactive maze editor
- Real-time algorithm visualization
- Performance metrics display
- Algorithm selection dropdown
- Benchmark comparison tool

### File Support
- ASCII maze files (0=path, 1=wall, S=start, G=goal)
- PNG image mazes (black=wall, white=path, green=start, red=goal)

## Building the Project

### Prerequisites
- CMake 3.10 or higher
- SFML 2.5 or higher
- C++17 compatible compiler

### Windows Build Instructions

1. Install SFML:
```bash
vcpkg install sfml:x64-windows
```

2. Create build directory and build:
```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=[path to vcpkg]/scripts/buildsystems/vcpkg.cmake ..
cmake --build . --config Release
```

### Linux Build Instructions

1. Install SFML:
```bash
sudo apt-get install libsfml-dev
```

2. Create build directory and build:
```bash
mkdir build
cd build
cmake ..
make
```

## Running the Application

### Main Application
```bash
# From build directory
./PathfindingBenchmark
```

### Running Tests
```bash
# From build directory
./PathfindingBenchmark_test
```

## Controls

### Maze Editing
- Left Mouse Button: Draw/erase walls
- Right Mouse Button: Place start/goal points
- Mouse Drag: Continue drawing/erasing

### Algorithm Control
- Space: Run current algorithm
- Tab: Switch between algorithms
- Dropdown Menu: Select specific algorithm

### Maze Operations
- R: Generate random maze
- C: Clear maze
- Ctrl+L: Load maze from file
- Ctrl+O: Save maze to file

### Drawing Modes
- W: Wall drawing mode
- S: Start point mode
- G: Goal point mode
- P: Path (eraser) mode

## File Formats

### ASCII Maze Format
```
S0110000001
0100111110
0100000010
0111111010
0000000010
0111111110
0100000000
0101111111
0100000001
0111111G01
```

### PNG Maze Format
- Black pixels (RGB: 0,0,0): Walls
- White pixels (RGB: 255,255,255): Paths
- Green pixels (RGB: 0,255,0): Start point
- Red pixels (RGB: 255,0,0): Goal point

## Benchmarking

The application includes a comprehensive benchmarking system that compares:
- Execution time
- Memory usage
- Nodes explored
- Path length
- Success rate

Results are exported to:
- CSV file for data analysis
- Markdown file for documentation

## Project Structure
```
.
├── include/
│   ├── Maze.hpp
│   ├── GUI.hpp
│   ├── Benchmark.hpp
│   └── pathfinders/
│       ├── Pathfinder.hpp
│       ├── AStar.hpp
│       ├── Dijkstra.hpp
│       ├── BFS.hpp
│       ├── DFS.hpp
│       ├── GreedyBestFirst.hpp
│       └── JPS.hpp
├── src/
│   ├── main.cpp
│   ├── Maze.cpp
│   ├── GUI.cpp
│   └── Benchmark.cpp
├── tests/
│   └── TestRunner.cpp
├── resources/
│   └── arial.ttf
├── CMakeLists.txt
└── README.md
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments

- SFML team for the graphics library
- CICR Robo-Soccer team for the project requirements
- Various algorithm implementations referenced from academic papers 