#include "scheduler/scheduler.hpp"
#include <iostream>
#include <vector>

int main() {
    std::cout << "===== Running Scheduler Test (Round Robin) =====\n";

    // 1. Initialize the scheduler with a time quantum of 4
    Scheduler rr_scheduler(4);

    // 2. Create some processes (PCBs)
    // In a real OS, these would be managed by a Process Manager
    ProcessControlBlock p1(1, 10); // Process 1, burst time = 10
    ProcessControlBlock p2(2, 7);  // Process 2, burst time = 7
    ProcessControlBlock p3(3, 4);  // Process 3, burst time = 4

    // 3. Add processes to the scheduler's ready queue
    rr_scheduler.addProcess(&p1);
    rr_scheduler.addProcess(&p2);
    rr_scheduler.addProcess(&p3);

    // 4. Run the scheduler
    rr_scheduler.run();

    std::cout << "\n===== Scheduler Test Completed =====\n";
    return 0;
}