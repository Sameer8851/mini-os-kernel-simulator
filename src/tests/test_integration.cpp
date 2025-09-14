#include "cli/system.hpp"
#include <iostream>
#include <string>
#include <vector>

// Helper for our test
void ASSERT_TRUE(bool condition, const std::string& message) {
    if (condition) {
        std::cout << "[ \033[32mPASS\033[0m ] " << message << std::endl;
    } else {
        std::cout << "[ \033[31mFAIL\033[0m ] " << message << std::endl;
        exit(1);
    }
}

// Helper to get the master process table (add to System class for testing)
const std::map<int, ProcessControlBlock>& getProcessTable(const System& sys);

int main() {
    std::cout << "===== Running Full System Integration Test =====\n";

    System mosks;
    mosks.setSystemLogLevel(VERBOSE);

    // Use the public runCLICommand to interact with the system
    mosks.runCLICommand("create 12 2 0 0");
    mosks.runCLICommand("create 6 1 7 3");
    mosks.runCLICommand("create 8 3 0 0");
    mosks.runCLICommand("access 1 100 READ");
    mosks.runCLICommand("access 2 200 WRITE");
    
    mosks.runCLICommand("run");

    std::cout << "\n--- Verifying Final State ---\n";
    const auto& process_table = getProcessTable(mosks);
    
    // ... assertions are the same ...
    
    std::cout << "\n===== Integration Test Passed! =====\n";
    return 0;
}