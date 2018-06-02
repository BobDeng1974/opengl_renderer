#include <iostream>

#include "scenes.hpp"


extern "C" {
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
    __declspec(dllexport) int NvOptimusEnablement = 1;
}



int main() {
    std::ios::sync_with_stdio(false);

    return opengl::scenes::inquisitor_skull();
}
