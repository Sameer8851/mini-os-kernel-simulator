#include "virtual_memory.hpp"
#include <iostream>

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

int main() {
    testMemoryProtection();
    return 0;
}