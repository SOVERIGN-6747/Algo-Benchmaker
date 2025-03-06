#include "GUI.hpp"
#include <stdexcept>
#include <iostream>

int main() {
    try {
        GUI app;
        app.run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
} 