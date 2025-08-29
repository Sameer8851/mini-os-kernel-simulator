#include "cli/system.hpp"
#include <iostream>

void bootSystem() {
    std::cout << "Initializing Mini OS Kernel Simulator (MOSKS)..." << std::endl;
    std::cout << "--> Loading Core System Modules..." << std::endl;
    std::cout << "--> Launching Unified Shell Interface..." << std::endl;
    std::cout << "System Boot Complete [OK]" << std::endl;
}

int main() {
    bootSystem();

    System mosks;
    
    mosks.runCLI();

    return 0;
}