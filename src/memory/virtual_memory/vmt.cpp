#include "virtual_memory.hpp"
#include <iostream>
#include <vector>
#include <string>
using namespace std;

// This test is specifically designed to verify the two-level paging system
void testMultiLevelPaging() {
    cout << "\n==========================================================\n";
    cout << "  Testing Multi-Level Paging System\n";
    cout << "==========================================================\n";

    // Setup: 4 frames total, using LRU policy for this test
    VirtualMemoryManager vmm(16, 4, ReplacementPolicy::LRU);
    
    // Set log level to VERBOSE to see the new logic in action
    vmm.setLogLevel(VERBOSE);

    // 1. Allocate a process using the new function signature
    vmm.allocateProcess(1);

    // 2. Access a page. This should create the first page table (for PDI 0).
    cout << "\n--> Accessing page 10 (PDI 0, PTI 10)\n";
    vmm.accessPage(1, 10);
    
    // 3. Access a page with a large VPN. Since 1100 > 1024 (our PAGE_TABLE_SIZE),
    //    this will force a directory miss and create a *second* page table (for PDI 1).
    cout << "\n--> Accessing page 1100 (PDI 1, PTI 76) - Expecting Directory Miss\n";
    vmm.accessPage(1, 1100);

    // 4. Access a few more pages to fill the frames and test replacements
    vmm.accessPage(1, 20);
    vmm.accessPage(1, 1110);
    vmm.accessPage(1, 30); // This should trigger a replacement

    // 5. Print the tables to verify the two-level structure
    vmm.printFrameTable();
    vmm.printPageTable();

    // 6. Free the process to test the new cleanup logic
    cout << "\n--> Freeing process 1\n";
    vmm.freeProcess(1);
    vmm.printFrameTable(); // Should show all frames are free
}

int main() {
    testMultiLevelPaging();
    return 0;
}