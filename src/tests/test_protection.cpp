#include "memory/virtual_memory/virtual_memory.hpp"
#include <iostream>
#include <random>

void testMemoryProtection() {
    std::cout << "\n==========================================================\n";
    std::cout << "  Testing Memory Protection System (R/W/X)\n";
    std::cout << "==========================================================\n";

    VirtualMemoryManager vmm(16, 4, ReplacementPolicy::FIFO);
    vmm.setLogLevel(VERBOSE);
    vmm.allocateProcess(1);

    // --- Test Case 1: Read-Only Page ---
    std::cout << "\n--> Setting permissions for VP 10 to: READ=Yes, WRITE=No\n";
    vmm.setPagePermissions(1, 10, true, false, false); // R=1, W=0, X=0

    // Access the page to load it into memory
    vmm.accessPage(1, 10, AccessType::READ);

    std::cout << "\n--> Attempting an ALLOWED action: Reading from VP 10...\n";
    vmm.accessPage(1, 10, AccessType::READ); // Should succeed

    std::cout << "\n--> Attempting a DISALLOWED action: Writing to VP 10...\n";
    vmm.accessPage(1, 10, AccessType::WRITE); // Should cause a protection fault

    // --- Test Case 2: Non-Executable Page ---
    std::cout << "\n--> Setting permissions for VP 20 to: READ=Yes, WRITE=Yes, EXECUTE=No\n";
    vmm.setPagePermissions(1, 20, true, true, false); // R=1, W=1, X=0

    // Access the page to load it
    vmm.accessPage(1, 20, AccessType::READ);

    std::cout << "\n--> Attempting an ALLOWED action: Writing to VP 20...\n";
    vmm.accessPage(1, 20, AccessType::WRITE); // Should succeed

    std::cout << "\n--> Attempting a DISALLOWED action: Executing VP 20...\n";
    vmm.accessPage(1, 20, AccessType::EXECUTE); // Should cause a protection fault
}

void stressTestRandomAccess() {
    std::cout << "\n--- Stress Testing with Random Accesses ---\n";
    VirtualMemoryManager vmm(128, 4, ReplacementPolicy::LRU); // More frames for a better test
    vmm.setLogLevel(NORMAL); // Keep output clean for the stress test

    vmm.allocateProcess(1);
    vmm.allocateProcess(2);

    // Setup for random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> proc_dist(1, 2); // Randomly choose process 1 or 2
    std::uniform_int_distribution<> page_dist(0, 2000); // Access pages up to ~2 page tables deep

    int num_accesses = 5000;
    std::cout << "Performing " << num_accesses << " random page accesses..." << std::endl;

    for (int i = 0; i < num_accesses; ++i) {
        int processId = proc_dist(gen);
        int virtualPage = page_dist(gen);
        vmm.accessPage(processId, virtualPage, AccessType::READ); // Simulate a read
    }

    std::cout << "Stress test completed without crashing." << std::endl;
    vmm.printFrameTable();
    std::cout << "Total page faults under stress: " << vmm.getPageFaults() << std::endl;
}

int main() {
    testMemoryProtection();
    stressTestRandomAccess();
    return 0;
}