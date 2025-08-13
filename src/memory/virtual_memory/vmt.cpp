#include "virtual_memory.hpp"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// Helper function to run a standardized test for a given policy and log level
void runTest(ReplacementPolicy policy, const string& policyName, LogLevel logLevel) {
    cout << "\n==========================================================\n";
    cout << "  Testing Policy: " << policyName << " | Log Level: " 
         << (logLevel == NORMAL ? "NORMAL" : (logLevel == VERBOSE ? "VERBOSE" : "DEBUG")) << "\n";
    cout << "==========================================================\n";

    // Setup: 4 frames total (16KB memory / 4KB page size)
    VirtualMemoryManager vmm(16, 4, policy);
    
    // Set the log level for this specific test run
    vmm.setLogLevel(logLevel);

    // We will use one process with 6 virtual pages (0-5)
    vmm.allocateProcess(1, 6);

    // A standard reference string to test the policies
    vector<int> referenceString = {0, 1, 2, 3, 4, 0, 1, 5, 0, 1, 2, 3, 4, 5};
    
    for(int page : referenceString) {
        vmm.accessPage(1, page);
    }
    cout << "\n";

    // Print final state
    cout << "--- Final State ---\n";
    vmm.printFrameTable();
    cout << "Total Page Faults: " << vmm.getPageFaults() << "\n";
}


int main() {
    cout << "===== Starting Automated Virtual Memory Tests =====\n";

    // Test the LRU policy with all three log levels
    runTest(ReplacementPolicy::LRU, "LRU", NORMAL);
    runTest(ReplacementPolicy::LRU, "LRU", VERBOSE);
    runTest(ReplacementPolicy::LRU, "LRU", DEBUG);

    // You can easily add tests for other policies here as well
    // For example, to test FIFO:
    // runTest(ReplacementPolicy::FIFO, "FIFO", NORMAL);
    // runTest(ReplacementPolicy::FIFO, "FIFO", VERBOSE);

    cout << "\n===== Automated Tests Completed =====\n";
    
    return 0;
}