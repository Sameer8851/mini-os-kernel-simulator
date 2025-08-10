#include "virtual_memory.hpp"
#include <iostream>

using namespace std;

void automatedTest() {
    cout << "Starting automated test...\n";

    int memorySize = 16; // KB
    int pageSize = 4;    // KB
    VirtualMemoryManager vmm(memorySize, pageSize, ReplacementPolicy::LRU);

    // Allocate process 1 with 5 pages
    vmm.allocateProcess(1, 5);

    // Access pages to fill memory and cause page faults
    vmm.accessPage(1, 0);
    vmm.accessPage(1, 1);
    vmm.accessPage(1, 2);
    vmm.accessPage(1, 3);

    // All 4 frames now used, next access should cause replacement
    vmm.accessPage(1, 4);  // Page fault + replacement expected

    // Access page 0 again to update its usage
    vmm.accessPage(1, 0);

    // Access another new page to cause another replacement
    vmm.accessPage(1, 5);

    // Print page table and frame table
    vmm.printPageTable();
    vmm.printFrameTable();

    // Print total page faults
    cout << "Total page faults: " << vmm.getPageFaults() << "\n";

    // Free process and print frame table to verify cleanup
    vmm.freeProcess(1);
    vmm.printFrameTable();

    cout << "Automated test completed.\n";
}

int main() {
    automatedTest();
    return 0;
}
