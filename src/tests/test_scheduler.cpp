#include "scheduler/scheduler.hpp"
#include <iostream>
#include <vector>

// This single test function can run a scenario with any policy
void runSchedulerTest(SchedulingPolicy policy) {
    // --- Setup ---
    Scheduler scheduler(policy, 4); // Time quantum of 4 for RR

    // --- Processes ---
    // A high-priority, short, I/O-bound process
    ProcessControlBlock p1(1, 6, 1, 5, 2); 
    // A medium-priority, long, CPU-bound process
    ProcessControlBlock p2(2, 12, 2, 0, 0);
    // A low-priority, medium-length process
    ProcessControlBlock p3(3, 8, 3, 0, 0);

    scheduler.addProcess(&p1);
    scheduler.addProcess(&p2);
    scheduler.addProcess(&p3);

    // --- Run Simulation ---
    scheduler.run();
}

// --- Test Runner Main Function ---
int main() {
    std::cout << "===== Running Unified Scheduler Tests =====\n";
    
    std::cout << "\n=============================================\n";
    std::cout << "  Testing Policy: Round Robin with I/O\n";
    std::cout << "=============================================\n";
    runSchedulerTest(SchedulingPolicy::ROUND_ROBIN);

    std::cout << "\n=============================================\n";
    std::cout << "  Testing Policy: Priority with I/O\n";
    std::cout << "=============================================\n";
    runSchedulerTest(SchedulingPolicy::PRIORITY);

    std::cout << "\n=============================================\n";
    std::cout << "  Testing Policy: SJF with I/O\n";
    std::cout << "=============================================\n";
    runSchedulerTest(SchedulingPolicy::SJF);

    std::cout << "\n===== All Scheduler Tests Completed =====\n";
    return 0;
}