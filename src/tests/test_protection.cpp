#include "memory/virtual_memory/virtual_memory.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>

// --- Test Framework ---
void ASSERT_TRUE(bool condition, const std::string& message) {
    if (condition) {
        std::cout << "[ \033[32mPASS\033[0m ] " << message << std::endl;
    } else {
        std::cout << "[ \033[31mFAIL\033[0m ] " << message << std::endl;
        exit(1);
    }
}

// --- Test Suites ---

void testProcessLifecycle(VirtualMemoryManager& vmm, std::map<int, ProcessControlBlock>& process_list) {
    std::cout << "\n--- Testing Process Lifecycle ---\n";
    
    // Create PCB and pass it to the VMM
    process_list.emplace(1, 1);
    ProcessControlBlock& pcb1 = process_list.at(1);
    vmm.allocateProcess(pcb1);

    // 1. Corrected Assertion: The directory should be EMPTY after initialization.
    ASSERT_TRUE(pcb1.page_directory.empty() == true, "Page directory should be empty on creation.");

    // 2. Access a page to populate the directory
    vmm.accessPage(pcb1, 100, AccessType::READ);
    
    // 3. New Assertion: The directory should NOT be empty after a page access.
    ASSERT_TRUE(pcb1.page_directory.empty() == false, "Page directory should not be empty after first access.");

    // 4. Test freeing the process
    vmm.freeProcess(pcb1);
    process_list.erase(1);
    ASSERT_TRUE(process_list.count(1) == 0, "Process 1 should not exist after being freed.");
}

void testPcbIntegrationAndReplacement(VirtualMemoryManager& vmm, std::map<int, ProcessControlBlock>& process_list) {
    std::cout << "\n--- Testing PCB Integration in Page Replacement ---\n";
    
    // Create and allocate processes
    process_list.emplace(10, 10);
    process_list.emplace(20, 20);
    ProcessControlBlock& pcb10 = process_list.at(10);
    ProcessControlBlock& pcb20 = process_list.at(20);
    vmm.allocateProcess(pcb10);
    vmm.allocateProcess(pcb20);

    // Fill all 4 frames
    vmm.accessPage(pcb10, 1, AccessType::READ);
    vmm.accessPage(pcb20, 2, AccessType::READ);
    vmm.accessPage(pcb10, 3, AccessType::READ);
    vmm.accessPage(pcb20, 4, AccessType::READ);

    // Trigger replacement (FIFO will evict P10, VP1)
    vmm.accessPage(pcb10, 5, AccessType::READ);

    int victim_pdi = 1 / PAGE_TABLE_SIZE;
    int victim_pti = 1 % PAGE_TABLE_SIZE;
    bool is_victim_invalid = !pcb10.page_directory.at(victim_pdi).pageTable->at(victim_pti).valid;
    ASSERT_TRUE(is_victim_invalid, "Victim page (P10, 1) should be marked invalid after eviction.");
}


// --- Test Runner Main Function ---

int main() {
    std::cout << "===== Running Virtual Memory Manager Unit Tests =====\n";
    
    // Setup VMM and a process list for each test scenario
    VirtualMemoryManager vmm_fifo(16, 4, ReplacementPolicy::FIFO);
    std::map<int, ProcessControlBlock> process_list_fifo;
    
    // Run tests
    testProcessLifecycle(vmm_fifo, process_list_fifo);
    testPcbIntegrationAndReplacement(vmm_fifo, process_list_fifo);

    std::cout << "\n===== All VMU tests passed! =====\n";
    return 0;
}